#pragma once

#include "Archetype.hpp"

class World
{
private:
    template <ComponentType... Components>
    static ArchetypeManager& GetManager()
    {
        static const Index index = ArchetypeManager::Find(Component::CreateID<Components...>());
        return ArchetypeManager::archetypes[index];
    }

    template <class T>
    struct EntityCreationHandler;

    template <class Ret, class T, typename... Components>
    struct EntityCreationHandler<Ret(T::*)(Components &...) const>
    {
        template <typename Lambda>
        static Entity Execute(Lambda lambda)
        {
            ArchetypeManager& archetype = World::GetManager<Components...>();
            Entity::Record& record = archetype.ReserveRecord();
            lambda(archetype.GetComponentArray<Components>()[record.row]...);
            return Entity(record.GetIndex());
        }
    };

    template <class T>
    struct EntityAccessorHandler;

    template <class Ret, class T, typename... Components>
    struct EntityAccessorHandler<Ret(T::*)(Components *...) const>
    {
        template <typename Lambda>
        static void Execute(ArchetypeManager& archetype, Index row, Lambda lambda)
        {
            lambda(archetype.GetComponent<Components>(row) ...);
        }
    };

    template <Component::IdType archetypeCacheID>
    struct ArchetypeLookupCache
    {
        static inline Index lastIndexChecked = 0;
        static inline Index matchCount = 0;
        static inline Index* matchedIndices = nullptr;

        static void UpdateCache()
        {
            while (lastIndexChecked < ArchetypeManager::archetypeCount)
            {
                if (Component::Contains(ArchetypeManager::archetypes[lastIndexChecked].id, archetypeCacheID))
                {
                    matchedIndices = static_cast<Index*>(realloc(matchedIndices, (matchCount + 1) * sizeof(Index)));
                    matchedIndices[matchCount++] = lastIndexChecked;
                }
                lastIndexChecked++;
            }
        }

        template <typename Lambda>
        static void ForEachMatch(Lambda lambda)
        {
            UpdateCache();
            for (size_t i = 0; i < matchCount; i++)
            {
                ArchetypeManager::archetypes[i].ForEachRow(lambda);
            }
        }
    };

public:
    template <typename Lambda>
    static Entity CreateEntity(Lambda lambda)
    {
        return EntityCreationHandler<decltype(&Lambda::operator())>::Execute(lambda);
    }

    template <ComponentType... Components>
    static Entity CreateEntity()
    {
        ArchetypeManager& archetype = World::GetManager<Components...>();
        Entity::Record& record = archetype.ReserveRecord();
        ((new (&archetype.GetComponentArray<Components>()[record.row]) Components()), ...);
        return Entity(record.GetIndex());
    }

    template <typename Lambda>
    static bool AccessEntity(Entity& entity, Lambda lambda)
    {
        bool status = false;
        entity.AccessRecord([lambda, &status](Entity::Record& record)
        {
            ArchetypeManager& archetype = ArchetypeManager::archetypes[record.archetype];
            EntityAccessorHandler<decltype(&Lambda::operator())>::Execute(archetype, record.row, lambda);
            status = true;
        });
        return status;
    }
};
