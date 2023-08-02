#pragma once

#include <stdint.h>
#include <stddef.h>

#include "..\Utils\SatAlloc.hpp"
#include "..\Utils\TemplateUtils.hpp"

#include "Component.hpp"
#include "Entity.hpp"

struct ArchetypeManager
{
    friend class Entity;
    friend class World;

    static inline ArchetypeManager* archetypes = nullptr;
    static inline Index archetypeCount = 0;

    static Index Find(Component::IdType id)
    {
        for (size_t i = 0; i < archetypeCount; ++i)
        {
            if (archetypes[i].id == id)
            {
                return i;
            }
        }

        archetypes = static_cast<ArchetypeManager*>(realloc(archetypes, (archetypeCount + 1) * sizeof(ArchetypeManager)));
        new (&archetypes[archetypeCount]) ArchetypeManager(id);
        return archetypeCount++;
    }

    Index GetIndex() { return static_cast<Index>(this - archetypes); }

    Component::IdType id;

    using InternalIndex = uint8_t;
    static inline constexpr InternalIndex Unused = ~(InternalIndex(0));

    Index* recordIndices = nullptr;
    void** componentArrays = nullptr;
    InternalIndex internalIndex[Component::MaxComponents] = { Unused };
    Index capacity = 0;
    Index size = 0;

    char* GetComponentArray(size_t componentId) const
    {
        return static_cast<char*>(componentArrays[internalIndex[componentId]]);
    }

    template <ComponentType T>
    T* GetComponentArray() const
    {
        return static_cast<T*>(componentArrays[internalIndex[Component::ID<T>]]);
    }

    template <ComponentType T>
    T* GetComponent(Index row) const
    {
        auto index = internalIndex[Component::ID<T>];
        return (index == Unused) ? nullptr :
            &((static_cast<T*>(componentArrays[index]))[row]);
    }

    ArchetypeManager(Component::IdType id) : id(id)
    {
        uint8_t localComponentIndex = 0;
        Component::EachID(id, [this, &localComponentIndex](size_t componentId)
        {
            internalIndex[componentId] = localComponentIndex++;
        });
        componentArrays = static_cast<void**>(malloc(sizeof(void*) * localComponentIndex));
        for (size_t i = 0; i < localComponentIndex; i++)
        {
            componentArrays[i] = nullptr;
        }

    }

    Entity::Record& ReserveRecord()
    {
        Entity::Record& entityRecord = Entity::Record::Reserve();

        if (size >= capacity)
        {
            capacity = (capacity == 0) ? 2 : capacity * 2;
            recordIndices = static_cast<Index*>(realloc(recordIndices, sizeof(Index) * capacity));

            Component::EachIdAndSize(id, [this](size_t componentId, size_t componentSize)
            {
                uint8_t componentArrayIndex = internalIndex[componentId];
                componentArrays[componentArrayIndex] =
                    realloc(componentArrays[componentArrayIndex], componentSize * capacity);
            });
        }

        recordIndices[size] = entityRecord.GetIndex();

        entityRecord.archetype = static_cast<Index>(GetIndex());
        entityRecord.row = size++;
        entityRecord.referenceCounter = 0;

        return entityRecord;
    }

    void RemoveRow(Index row)
    {
        Index lastRow = size ? size - 1 : 0;
        if (row != lastRow)
        {

            Component::EachIdAndSize(id, [this, row, lastRow](size_t componentId, size_t componentSize)
            {
                char* componentArray = static_cast<char*>(GetComponentArray(componentId));
                memcpy(
                    componentArray + (componentSize * row),
                    componentArray + (componentSize * lastRow),
                    componentSize
                );
            });

            Entity::Record::records[recordIndices[row]].Invalidate();
            Entity::Record::records[recordIndices[lastRow]].row = row;
            recordIndices[row] = recordIndices[lastRow];
        }
        if (size) size--;
    }

    Entity::Record& MoveEntity(ArchetypeManager* sourceArchetype, size_t sourceRow)
    {
        Entity::Record& record = ReserveRecord();

        Component::EachCommonIdAndSize(id, sourceArchetype->id, [this, &record, sourceArchetype, sourceRow](size_t componentId, size_t componentSize)
        {
            memcpy(
                GetComponentArray(componentId) + (componentSize * record.row),
                sourceArchetype->GetComponentArray(componentId) + (componentSize * sourceRow),
                componentSize
            );
        });
        sourceArchetype->RemoveRow(sourceRow);
        return record;
    }

    template <ComponentType T>
    static inline T* arrayAccessor;

    template <ComponentType T>
    void CacheAccessor() const
    {
        arrayAccessor<T> = static_cast<T*>(componentArrays[internalIndex[Component::ID<T>]]);
    }

    template <class T>
    struct ForEachRowHandler;

    template <class Ret, class L, typename... Components>
    struct ForEachRowHandler<Ret(L::*)(Components&...)>
    {
        template <typename Lambda>
        static void Execute(ArchetypeManager* archetype, Lambda lambda)
        {
            ((archetype->CacheAccessor<Components>()), ...);

            for (size_t row = 0; row < archetype->size; row++)
            {
                lambda(ArchetypeManager::arrayAccessor<Components>[row]...);
            }
        }
    };

    template <typename Lambda>
    void ForEachRow(Lambda lambda)
    {
        ForEachRowHandler<decltype(&Lambda::operator())>::Execute(this, lambda);
    }
};
