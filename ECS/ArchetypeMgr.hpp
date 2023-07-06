#pragma once
#include <stdint.h>

#include "ArchetypeId.hpp"

#include "stddef.h"
#include "..\Utils\RBTree.hpp"
#include "..\Utils\Vector.hpp"
#include "..\Utils\SatAlloc.hpp"

using MgrIndex = int16_t;
using TypeRow = uint16_t;

template <typename... SupportedTypes>
class ArchetypeMgr
{
public:
    using ArchetypeIdST = ArchetypeId<SupportedTypes...>;
    const ArchetypeIdST type;

private:
    const size_t dataSize;
    size_t capacity;
    size_t size;
    size_t offsets[ArchetypeIdST::NonTagComponentCount()] = {};
    void *entities;

    char *EntityAddress(const TypeRow &row) const
    {
        return (char *)entities + (dataSize * row);
    }

    char *EntityComponents(const TypeRow &row) const
    {
        return EntityAddress(row) + sizeof(size_t);
    }

public:
    ArchetypeMgr(const ArchetypeIdST &a) : type(a),
                                           dataSize(sizeof(size_t) + type.Size()),
                                           capacity(0),
                                           size(0),
                                           entities(nullptr)
    {
        if (!type.IsEmpty())
            ((!std::is_empty_v<SupportedTypes> ? offsets[ArchetypeIdST::template ComponentIndex<SupportedTypes>()] = type.template GetOffset<SupportedTypes>() : false), ...);
    }

    TypeRow Add(const size_t &entityId)
    {
        void *data = nullptr;
        if (size >= capacity)
        {
            capacity = (capacity == 0) ? 1 : capacity * 2;
            entities = lwram::realloc(entities, dataSize * capacity);
        }

        data = (char *)entities + (dataSize * size);
        size++;

        if (data)
        {
            memset(data, 0, dataSize);
            *(size_t *)data = entityId;
        }

        return size - 1;
    }

    uint16_t Last() const { return size ? size - 1 : 0; }

    uint16_t Size() const { return size; }

    bool Empty() const { return size == 0; }

    size_t GetId(const TypeRow &row) const
    {
        return *((size_t *)EntityAddress(row));
    }

    size_t *RemoveRow(const TypeRow &row)
    {
        uint16_t last = Last();
        if (!Empty())
            size--;

        if (row == last)
            return nullptr;

        memcpy(EntityAddress(row), EntityAddress(last), dataSize);
        return (size_t *)EntityAddress(row);
    }

    template <typename Component>
    Component *Get(const TypeRow &row) const
    {
        if constexpr (std::is_empty_v<Component>)
            return nullptr;
        else
            return (Component *)(EntityComponents(row) + offsets[ArchetypeIdST::template ComponentIndex<Component>()]);
    }

private:
    static inline Vector<ArchetypeMgr> managers;
    static inline RBTree<ArchetypeIdST, MgrIndex> managerMap;

public:
    static MgrIndex Find(const ArchetypeIdST &a)
    {
        if (MgrIndex *index = managerMap.Search(a))
            return *index;

        managers.push_back(ArchetypeMgr(a));
        managerMap.Insert(a, managers.size() - 1);
        return managers.size() - 1;
    }

    static ArchetypeMgr *Access(const MgrIndex &index)
    {
        return managers[index];
    }

    static size_t Count()
    {
        return managers.size();
    }
};
