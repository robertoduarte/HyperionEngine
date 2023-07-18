#pragma once
#include <stdint.h>

#include "ArchetypeId.hpp"

#include "stddef.h"
#include "..\Utils\RBTree.hpp"
#include "..\Utils\Vector.hpp"
#include "..\Utils\SatAlloc.hpp"

using MgrIndex = int16_t;
using TypeRow = uint16_t;

class ArchetypeMgr
{
private:
    size_t dataSize;
    size_t capacity;
    size_t size;
    size_t offsets[ArchetypeId::maxComponents] = {};
    void *entities;

    char *EntityAddress(const TypeRow &row) const { return (char *)entities + (dataSize * row); }
    char *EntityComponents(const TypeRow &row) const { return EntityAddress(row) + sizeof(size_t); }

public:
    const ArchetypeId type;
    ArchetypeMgr(const ArchetypeId &a) : type(a),
                                         capacity(0),
                                         size(0),
                                         entities(nullptr)
    {
        dataSize = type.GetSizeAndSetOffsets(offsets);
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

    size_t GetId(const TypeRow &row) const { return *((size_t *)EntityAddress(row)); }

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

    template <typename T>
    T *Get(const TypeRow &row) const { return (T *)(EntityComponents(row) + offsets[Component::Id<T>]); }

    void *Get(const size_t componentId, const TypeRow &row) const { return EntityComponents(row) + offsets[componentId]; }

private:
    static inline Vector<ArchetypeMgr> managers;
    static inline RBTree<ArchetypeId, MgrIndex> managerMap;

public:
    static MgrIndex Find(const ArchetypeId &a)
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
