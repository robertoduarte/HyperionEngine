#pragma once

#include <stdint.h>
#include <stddef.h>

#include "..\Utils\SatAlloc.hpp"
#include "..\Utils\TemplateUtils.hpp"

#include "EntityRecord.hpp"

namespace Hyperion::ECS
{
    /**
     * @brief A concept that defines the requirements for a valid component type.
     * A component type must be trivial and non-empty.
     * @tparam T The component type.
     */
    template <typename T>
    concept ComponentType = std::is_trivial_v<T> && (!std::is_empty_v<T>);

    /**
     * @brief Manages archetypes for entities in an ECS (Entity-Component-System).
     */
    class ArchetypeManager
    {
        friend class EntityReference;
        friend class World;

        using BinaryType = size_t;

        static inline constexpr size_t maxComponents = sizeof(BinaryType) * CHAR_BIT;
        static inline size_t componentSizes[maxComponents] = { 0 };

        /**
         * @brief A binary identifier for a specific component type.
         * @tparam T The component type.
         */
        template <ComponentType T>
            requires(TypeInfo::ID<T> < maxComponents)
        static inline const BinaryType ComponentBinaryID = []()
        {
            componentSizes[TypeInfo::ID<T>] = sizeof(T);
            return (BinaryType)1 << TypeInfo::ID<T>;
        }();

        static inline ArchetypeManager* managers = nullptr;
        static inline size_t managerCount = 0;

        static size_t Find(BinaryType id)
        {
            for (size_t i = 0; i < managerCount; ++i)
            {
                if (managers[i].id == id)
                {
                    return i;
                }
            }

            managers = static_cast<ArchetypeManager*>(realloc(managers, (managerCount + 1) * sizeof(ArchetypeManager)));
            new (&managers[managerCount]) ArchetypeManager(id);
            return managerCount++;
        }

        /**
         * @brief Helper struct for managing components.
         * @tparam T The component types.
         */
        template <typename... T>
        struct HelperImplementation
        {
            static inline const BinaryType id = ((ComponentBinaryID<T>) | ...);

            /**
             * @brief Get the instance of the archetype manager.
             * @return The instance.
             */
            static auto GetInstance()
            {
                static const size_t index = ArchetypeManager::Find(id);
                return &ArchetypeManager::managers[index];
            }

            /**
             * @brief Add component to the binary identifier.
             * @param sourceID The binary identifier to add to.
             * @return The updated binary identifier.
             */
            static BinaryType AddTo(BinaryType sourceID)
            {
                return sourceID | id;
            }

            /**
             * @brief Remove component from the binary identifier.
             * @param sourceID The binary identifier to remove from.
             * @return The updated binary identifier.
             */
            static BinaryType RemoveFrom(BinaryType sourceID)
            {
                return sourceID & ~id;
            }
        };

        /**
         * @brief Template alias for HelperImplementation.
         * @tparam Ts The component types.
         */
        template <class... Ts>
        using Helper = instantiate_t<HelperImplementation, sorted_list_t<list<Ts...>>>;

        /**
         * @brief Struct for caching lookup results.
         * @tparam T The component types.
         */
        template <typename... T>
        struct LookupCacheImplementation
        {
            static inline Index lastIndexChecked = 0;
            static inline Index matchCount = 0;
            static inline Index* matchedIndices = nullptr;

            /**
             * @brief Update the cache.
             */
            static void Update()
            {
                while (lastIndexChecked < ArchetypeManager::managerCount)
                {
                    if (ArchetypeManager::managers[lastIndexChecked].Contains(Helper<T...>::id))
                    {
                        matchedIndices = static_cast<Index*>(realloc(matchedIndices, (matchCount + 1) * sizeof(Index)));
                        matchedIndices[matchCount] = lastIndexChecked;
                        matchCount++;
                    }
                    lastIndexChecked++;
                }
            }
        };

        /**
         * @brief Template alias for LookupCacheImplementation.
         * @tparam Ts The component types.
         */
        template <class... Ts>
        using LookupCache = instantiate_t<LookupCacheImplementation, sorted_list_t<list<Ts...>>>;

        Index GetIndex() { return static_cast<Index>(this - managers); }

        BinaryType id;

        using InternalIndex = uint8_t;
        static inline constexpr InternalIndex Unused = ~(InternalIndex(0));

        Index* recordIndices = nullptr;
        void** componentArrays = nullptr;
        InternalIndex internalIndex[maxComponents] = { Unused };
        Index capacity = 0;
        Index size = 0;

        /**
         * @brief Iterate over each component in a binary identifier.
         * @param id The binary identifier.
         * @param lambda The lambda function to apply to each component.
         */
        template <typename Lambda>
        static void EachComponent(BinaryType id, Lambda lambda)
        {
            size_t componentId = 0;
            do
            {
                if (1 & id)
                {
                    lambda(componentId);
                }
                componentId++;
            } while (id >>= 1);
        }

        /**
         * @brief Iterate over common components in two binary identifiers.
         * @param idA The first binary identifier.
         * @param idB The second binary identifier.
         * @param lambda The lambda function to apply to common components.
         */
        template <typename Lambda>
        static void EachCommonComponent(BinaryType idA, BinaryType idB, Lambda lambda)
        {
            size_t componentId = 0;
            do
            {
                if (1 & idA & idB)
                {
                    lambda(componentId);
                }
                componentId++;
            } while ((idA >>= 1) && (idB >>= 1));
        }

        /**
         * @brief Check if the archetype contains a set of components.
         * @param expected The binary identifier of expected components.
         * @return true if the archetype contains the expected components, false otherwise.
         */
        bool Contains(BinaryType expected) { return (id & expected) == expected; }

        /**
         * @brief Get a pointer to a component array.
         * @param componentId The component identifier.
         * @return A pointer to the component array.
         */
        char* GetComponentArray(size_t componentId) const
        {
            return static_cast<char*>(componentArrays[internalIndex[componentId]]);
        }

        /**
         * @brief Get a strongly-typed pointer to a component array.
         * @tparam T The component type.
         * @return A pointer to the component array.
         */
        template <typename T>
        T* GetComponentArray() const
        {
            return static_cast<T*>(componentArrays[internalIndex[TypeInfo::ID<T>]]);
        }

        /**
         * @brief Get a strongly-typed pointer to a component within a row.
         * @tparam T The component type.
         * @param row The row index.
         * @return A pointer to the component.
         */
        template <typename T>
        T* GetComponent(Index row) const
        {
            auto index = internalIndex[TypeInfo::ID<T>];
            return (index == Unused) ? nullptr :
                &((static_cast<T*>(componentArrays[index]))[row]);
        }

        /**
         * @brief Construct an ArchetypeManager with a given binary identifier.
         * @param newId The binary identifier.
         */
        ArchetypeManager(BinaryType newId) : id(newId)
        {
            uint8_t localComponentCount = 0;
            EachComponent(newId, [this, &localComponentCount](size_t componentId)
            {
                internalIndex[componentId] = localComponentCount++;
            });
            componentArrays = static_cast<void**>(malloc(sizeof(void*) * localComponentCount));
            for (size_t i = 0; i < localComponentCount; i++)
            {
                componentArrays[i] = nullptr;
            }
        }

        /**
         * @brief Reserve an EntityRecord within the archetype.
         * @return The reserved EntityRecord.
         */
        EntityRecord& ReserveRecord()
        {
            EntityRecord& entityRecord = EntityRecord::Reserve();

            if (size >= capacity)
            {
                capacity = (capacity == 0) ? 2 : (capacity * 2) - (capacity / 2);
                recordIndices = static_cast<Index*>(realloc(recordIndices, sizeof(Index) * capacity);

                EachComponent(id, [this](size_t componentId)
                {
                    const uint8_t componentArrayIndex = internalIndex[componentId];
                    componentArrays[componentArrayIndex] =
                        realloc(componentArrays[componentArrayIndex], componentSizes[componentId] * capacity);
                });
            }

            recordIndices[size] = entityRecord.GetIndex();

            entityRecord.archetype = static_cast<Index>(GetIndex());
            entityRecord.row = size++;

            return entityRecord;
        }

        /**
         * @brief Remove a row from the archetype.
         * @param row The row index to remove.
         */
        void RemoveRow(Index row)
        {
            if (size) size--;
            Index lastRow = size;
            if (row != lastRow)
            {
                EachComponent(id, [this, row, lastRow](size_t componentId)
                {
                    char* componentArray = static_cast<char*>(GetComponentArray(componentId));
                    const size_t componentSize = componentSizes[componentId];
                    memcpy(
                        componentArray + (componentSize * row),
                        componentArray + (componentSize * lastRow),
                        componentSize
                    );
                });

                EntityRecord::records[recordIndices[row]].Release();
                EntityRecord::records[recordIndices[lastRow]].row = row;
                recordIndices[row] = recordIndices[lastRow];
            }
        }

        /**
         * @brief Move an entity from another archetype into this archetype.
         * @param sourceArchetype The source archetype.
         * @param sourceRow The source row within the source archetype.
         * @return The EntityRecord for the moved entity.
         */
        EntityRecord& MoveEntity(ArchetypeManager* sourceArchetype, size_t sourceRow)
        {
            EntityRecord& record = ReserveRecord();
            EachCommonComponent(id, sourceArchetype->id, [this, &record, sourceArchetype, sourceRow](size_t componentId)
            {
                const size_t componentSize = componentSizes[componentId];
                memcpy(
                    GetComponentArray(componentId) + (componentSize * record.row),
                    sourceArchetype->GetComponentArray(componentId) + (componentSize * sourceRow),
                    componentSize
                );
            });
            sourceArchetype->RemoveRow(sourceRow);
            return record;
        }
    };
}
