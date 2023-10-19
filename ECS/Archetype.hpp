#pragma once

#include <stdint.h>
#include <stddef.h>

#include "..\Utils\SatAlloc.hpp"
#include "..\Utils\TemplateUtils.hpp"

#include "EntityRecord.hpp"

namespace Hyperion::ECS
{
    template <typename T>
    concept ComponentType = std::is_trivial_v<T> && (!std::is_empty_v<T>);

    class ArchetypeManager
    {
        friend class EntityReference;
        friend class World;

        using BinaryType = size_t;

        static inline constexpr size_t maxComponents = sizeof(BinaryType) * CHAR_BIT;
        static inline size_t componentSizes[maxComponents] = { 0 };

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

        template <typename... T>
        struct HelperImplementation
        {
            static inline const BinaryType id = ((ComponentBinaryID<T>) | ...);

            static auto GetInstance()
            {
                static const size_t index = ArchetypeManager::Find(id);
                return &ArchetypeManager::managers[index];
            }

            static BinaryType AddTo(BinaryType sourceID)
            {
                return sourceID | id;
            }

            static BinaryType RemoveFrom(BinaryType sourceID)
            {
                return sourceID & ~id;
            }
        };

        template <class... Ts>
        using Helper = instantiate_t<HelperImplementation, sorted_list_t<list<Ts...>>>;

        template <typename... T>
        struct LookupCacheImplementation
        {
            static inline Index lastIndexChecked = 0;
            static inline Index matchCount = 0;
            static inline Index* matchedIndices = nullptr;

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

        bool Contains(BinaryType expected) { return (id & expected) == expected; }

        char* GetComponentArray(size_t componentId) const
        {
            return static_cast<char*>(componentArrays[internalIndex[componentId]]);
        }

        template <typename T>
        T* GetComponentArray() const
        {
            return static_cast<T*>(componentArrays[internalIndex[TypeInfo::ID<T>]]);
        }

        template <typename T>
        T* GetComponent(Index row) const
        {
            auto index = internalIndex[TypeInfo::ID<T>];
            return (index == Unused) ? nullptr :
                &((static_cast<T*>(componentArrays[index]))[row]);
        }

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

        EntityRecord& ReserveRecord()
        {
            EntityRecord& entityRecord = EntityRecord::Reserve();

            if (size >= capacity)
            {
                capacity = (capacity == 0) ? 2 : (capacity * 2) - (capacity / 2);
                recordIndices = static_cast<Index*>(realloc(recordIndices, sizeof(Index) * capacity));

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
