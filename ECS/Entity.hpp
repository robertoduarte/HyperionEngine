#pragma once

#include "ArchetypeMgr.hpp"
#include "..\Utils\IdTracker.hpp"
#include "yaul.h"

static constexpr size_t EntityCapacity = 512;

class Entity
{
    // Static Stuff
private:
    static Entity invalidEntity;
    static Entity entities[EntityCapacity];
    static inline FixedIdTracker<EntityCapacity> idTracker;

public:
    static Entity &Create(const ArchetypeId &archetype = ArchetypeId())
    {

        size_t id;
        if (idTracker.AssingId(id))
        {
            Entity &entity = entities[id];
            entity.managerIndex = ArchetypeMgr::Find(archetype);
            entity.row = ArchetypeMgr::Access(entity.managerIndex)->Add(id);
            return entity;
        }

        return invalidEntity;
    }

    template <typename... T>
    static Entity &Create()
    {
        return Create(ArchetypeId::BuildArchetypeId<T...>());
    }

    template <typename... T>
    static Entity &Create(const T &...component)
    {
        Entity &entity = Create<T...>();
        if (entity.IsValid())
        {
            ((*entity.Get<T, false>() = component), ...);
        }

        return entity;
    }

    static Entity &GetEntity(const size_t &id)
    {
        if (id < EntityCapacity)
            return entities[id];
        else
            return invalidEntity;
    }

    static void Delete(const size_t &id)
    {
        if (id < EntityCapacity)
        {
            entities[id].Remove();
            idTracker.FreeId(id);
        }
    }

    // Member Stuff
private:
    MgrIndex managerIndex;
    TypeRow row;

    void CopyComponents(const Entity &srcEntity)
    {
        auto thisType = GetType().id;
        auto otherType = srcEntity.GetType().id;
        size_t componentId = 0;
        while (thisType)
        {
            if ((thisType & 1) && (otherType & 1))
            {
                memcpy(Get(componentId),
                       srcEntity.Get(componentId),
                       Component::SizeFromId(componentId));
            }
            componentId++;
            thisType >>= 1;
            otherType >>= 1;
        }
    }

    void Replace(const Entity &srcEntity)
    {
        Remove();
        managerIndex = srcEntity.managerIndex;
        row = srcEntity.row;
    }

    Entity() : managerIndex(-1), row(0){};

    Entity(const ArchetypeId &a, const Entity &srcEntity)
    {
        managerIndex = ArchetypeMgr::Find(a);
        row = ArchetypeMgr::Access(managerIndex)->Add(srcEntity.GetId());
        CopyComponents(srcEntity);
    }

    void Remove()
    {
        if (IsValid())
        {
            ArchetypeMgr *manager = ArchetypeMgr::Access(managerIndex);
            if (size_t *id = manager->RemoveRow(row))
                entities[*id].row = row;
        }
    }

    template <bool check = false>
    void *Get(const size_t componentId) const
    {
        if constexpr (check)
            return IsValid() ? ArchetypeMgr::Access(managerIndex)->Get(componentId, row) : nullptr;
        else
            return ArchetypeMgr::Access(managerIndex)->Get(componentId, row);
    }

public:
    const ArchetypeId &GetType() const
    {
        return ArchetypeMgr::Access(managerIndex)->type;
    }

    size_t GetId() const
    {
        return ArchetypeMgr::Access(managerIndex)->GetId(row);
    }

    bool IsValid() const
    {
        return managerIndex != -1;
    }

    template <typename T, bool check = false>
    T *Get() const
    {
        if constexpr (check)
            return IsValid() ? ArchetypeMgr::Access(managerIndex)->Get<T>(row) : nullptr;
        else
            return ArchetypeMgr::Access(managerIndex)->Get<T>(row);
    }

    template <typename T>
    Entity &AddComponent()
    {
        if (IsValid())
        {
            ArchetypeId component = ArchetypeId::BuildArchetypeId<T>();
            ArchetypeId type = GetType();
            if (!type.Contains(component))
                Replace(Entity(type + component, *this));
        }
        return *this;
    }

    template <typename... T>
    Entity &AddComponent(const T &...component)
    {
        if (IsValid())
        {
            constexpr ArchetypeId components = ArchetypeId::BuildArchetypeId<T...>();
            ArchetypeId type = GetType();
            if (!type.template Contains(components))
                Replace(Entity(type + components, *this));

            ((*Get<T>() = component), ...);
        }
        return *this;
    }

    template <typename... T>
    Entity &RemoveComponents()
    {
        if (IsValid())
        {
            const ArchetypeId components = ArchetypeId::BuildArchetypeId<T...>();
            ArchetypeId type = GetType();
            if (type.template Contains(components))
                Replace(Entity(type - components, *this));
        }
        return *this;
    }

private:
    template <class T>
    struct ForEachHelper;

    template <class Ret, class T, typename... C>
    struct ForEachHelper<Ret (T::*)(C &...) const>
    {
        static ArchetypeId GetType()
        {
            return ArchetypeId::BuildArchetypeId<C...>();
        }

        template <typename Lambda>
        static void Execute(ArchetypeMgr *manager, const TypeRow &row, Lambda lambda)
        {
            lambda(*manager->Get<C>(row)...);
        }
    };

public:
    template <typename Lambda>
    static void ForEach(Lambda lambda)
    {
        auto helper = ForEachHelper<decltype(&Lambda::operator())>();
        auto type = helper.GetType();
        for (size_t i = 0; i < ArchetypeMgr::Count(); i++)
        {
            ArchetypeMgr *manager = ArchetypeMgr::Access(i);

            if (manager->type.Contains(type) && !manager->Empty())
            {
                TypeRow row = manager->Size();
                do
                {
                    row--;
                    helper.Execute(manager, row, lambda);
                } while (row != 0);
            }
        }
    };
};

inline Entity Entity::invalidEntity;
inline Entity Entity::entities[EntityCapacity];
