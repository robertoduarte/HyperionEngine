#pragma once

#include <stdint.h>
#include <stddef.h>

#include "..\Utils\SatAlloc.hpp"
#include "..\Utils\std\vector.h"

#include "EntityRecord.hpp"
#include "Component.hpp"

namespace Hyperion::ECS
{
    /**
     * @brief A concept that defines the requirements for a valid component type.
     * A component type must be non-empty.
     * @tparam T The component type.
     */
    template <typename T>
    concept ComponentType = !std::is_empty_v<T>;

    /**
     * @brief Manages archetypes for entities in an ECS (Entity-Component-System).
     */
    class ArchetypeManager
    {
        friend class EntityReference;
        friend class World;

        static inline std::vector<ArchetypeManager> managers;

        /**
         * @brief Finds the index of the archetype manager associated with a specific component binary identifier.
         * @param id The binary identifier of the components.
         * @return The index of the archetype manager.
         */
        static size_t Find(Component::BinaryId id)
        {
            size_t size = managers.size();

            for (size_t i = 0; i < size; ++i)
            {
                if (managers[i].id == id) { return i; }
            }

            managers.push_back(std::move(ArchetypeManager(id)));

            return size;
        }

        /**
         * @brief Helper struct for managing components.
         * @tparam T The component types.
         */
        template <typename... T>
        struct HelperImplementation
        {
            static inline Component::BinaryId id = (Component::IdBinary<T> | ...);

            /**
             * @brief Get the instance of the archetype manager.
             * @return The instance.
             */
            static ArchetypeManager& GetInstance()
            {
                static const size_t index = ArchetypeManager::Find(id);
                return ArchetypeManager::managers[index];
            }

            /**
             * @brief Add component to the binary identifier.
             * @param sourceID The binary identifier to add to.
             * @return The updated binary identifier.
             */
            static Component::BinaryId AddTo(Component::BinaryId sourceID)
            {
                return sourceID | id;
            }

            /**
             * @brief Remove component from the binary identifier.
             * @param sourceID The binary identifier to remove from.
             * @return The updated binary identifier.
             */
            static Component::BinaryId RemoveFrom(Component::BinaryId sourceID)
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
            static inline size_t lastIndexChecked = 0;
            static inline std::vector<uint16_t> matchedIndices;

            /**
             * @brief Update the cache.
             */
            static void Update()
            {
                if (lastIndexChecked < ArchetypeManager::managers.size())
                {
                    auto cacheIterator = ArchetypeManager::managers.begin() + lastIndexChecked;
                    while (cacheIterator != ArchetypeManager::managers.end())
                    {
                        ArchetypeManager& manager = *cacheIterator;
                        if (manager.Contains(Helper<T...>::id))
                        {
                            matchedIndices.push_back(lastIndexChecked);
                        }
                        ++cacheIterator;
                        ++lastIndexChecked;
                    }
                }
            }
        };

        /**
         * @brief Template alias for LookupCacheImplementation.
         * @tparam Ts The component types.
         */
        template <class... Ts>
        using LookupCache = instantiate_t<LookupCacheImplementation, sorted_list_t<list<Ts...>>>;

        Index GetIndex() { return static_cast<Index>(this - &(*managers.begin())); }

        Component::BinaryId id;

        using InternalIndex = uint8_t;
        static inline constexpr InternalIndex Unused = ~(InternalIndex(0));

        Index* recordIndices = nullptr;
        void** componentArrays = nullptr;
        InternalIndex internalIndex[Component::MaxComponentTypes] = { Unused };
        Index capacity = 0;
        Index size = 0;

        /**
         * @brief Iterate over each component in a binary identifier.
         * @param id The binary identifier.
         * @param lambda The lambda function to apply to each component.
         */
        template <typename Lambda>
        static void EachComponent(Component::BinaryId id, Lambda lambda)
        {
            size_t componentId = 0;
            do
            {
                if (1 & id) { lambda(componentId); }
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
        static void EachCommonComponent(Component::BinaryId idA, Component::BinaryId idB, Lambda lambda)
        {
            size_t componentId = 0;
            do
            {
                if (1 & idA & idB) { lambda(componentId); }
                componentId++;
            } while ((idA >>= 1) && (idB >>= 1));
        }

        /**
         * @brief Check if the archetype contains a set of components.
         * @param expected The binary identifier of expected components.
         * @return true if the archetype contains the expected components, false otherwise.
         */
        bool Contains(Component::BinaryId expected) { return (id & expected) == expected; }

        /**
         * @brief Get a strongly-typed pointer to a component array.
         * @tparam T The component type.
         * @return A pointer to the component array.
         */
        template <typename T>
        T* GetComponentArray() const
        {
            return static_cast<T*>(componentArrays[internalIndex[Component::Id<T>]]);
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

    public:
        /**
         * @brief Default constructor.
         */
        ArchetypeManager() = default;

        /**
         * @brief Move constructor.
         * @param other The other ArchetypeManager to move.
         */
        ArchetypeManager(ArchetypeManager&& other) noexcept
            : id(std::move(other.id)),
            recordIndices(std::move(other.recordIndices)),
            componentArrays(std::move(other.componentArrays)),
            capacity(std::move(other.capacity)),
            size(std::move(other.size))
        {
            for (size_t i = 0; i < Component::MaxComponentTypes; ++i)
            {
                internalIndex[i] = std::move(other.internalIndex[i]);
                other.internalIndex[i] = Unused;
            }

            // Reset the source object
            other.id = 0;
            other.recordIndices = nullptr;
            other.componentArrays = nullptr;
            other.capacity = 0;
            other.size = 0;
        }

        /**
         * @brief Move assignment operator.
         * @param other The other ArchetypeManager to move.
         * @return Reference to this ArchetypeManager after the move.
         */
        ArchetypeManager& operator=(ArchetypeManager&& other) noexcept
        {
            if (this != &other)
            {
                id = std::move(other.id);
                recordIndices = std::move(other.recordIndices);
                componentArrays = std::move(other.componentArrays);
                capacity = std::move(other.capacity);
                size = std::move(other.size);

                for (size_t i = 0; i < Component::MaxComponentTypes; ++i)
                {
                    internalIndex[i] = std::move(other.internalIndex[i]);
                    other.internalIndex[i] = Unused;
                }

                // Reset the source object
                other.id = 0;
                other.recordIndices = nullptr;
                other.componentArrays = nullptr;
                other.capacity = 0;
                other.size = 0;
            }

            return *this;
        }

    private:
        /**
         * @brief Construct an ArchetypeManager with a given binary identifier.
         * @param newId The binary identifier.
         */
        ArchetypeManager(Component::BinaryId newId) : id(newId)
        {
            uint8_t localComponentCount = 0;
            EachComponent(newId, [this, &localComponentCount](size_t componentId)
            {
                internalIndex[componentId] = localComponentCount++;
            });

            componentArrays = new void* [localComponentCount]();
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
                recordIndices = static_cast<Index*>(realloc(recordIndices, sizeof(Index) * capacity));

                EachComponent(id, [this](const size_t& componentId)
                {
                    Component::ResizeArray(componentId, &componentArrays[internalIndex[componentId]], capacity, size);
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
                    void* arrayPtr = componentArrays[internalIndex[componentId]];
                    Component::MoveElement(componentId, arrayPtr, row, arrayPtr, lastRow);
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
                void* arrayPtr = componentArrays[internalIndex[componentId]];
                void* srcArrayPtr = sourceArchetype->componentArrays[sourceArchetype->internalIndex[componentId]];
                Component::MoveElement(componentId, arrayPtr, record.row, srcArrayPtr, sourceRow);
            });
            sourceArchetype->RemoveRow(sourceRow);
            return record;
        }
    };
}
