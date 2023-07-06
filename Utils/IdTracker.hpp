#pragma once

#include "HierarchicalBitset.hpp"

template <size_t IdCapacity>
class FixedIdTracker
{
    size_t last = 0;
    FixedHierarchicalBitset<IdCapacity> recycleBin;

    bool InUsedRange(size_t id) const { return id < last; }

public:
    static constexpr size_t capacity = IdCapacity;

    bool IsUsed(size_t id) { return InUsedRange(id) && !recycleBin.Get(id); }

    void FreeId(size_t id)
    {
        if (InUsedRange(id))
        {
            if (id == last - 1)
            {
                last--;
                recycleBin.Clear(id);
            }
            else
            {
                recycleBin.Set(id);
            }
        }
    }

    bool AssingId(size_t &id)
    {
        if (last < capacity)
        {
            id = last++;
            return true;
        }

        if (recycleBin.LookupSetPos(id))
        {
            recycleBin.Clear(id);
            return true;
        }

        return false;
    }
};

class IdTracker
{
    size_t capacity = 0;
    size_t last = 0;
    HierarchicalBitset recycleBin;

    bool InUsedRange(size_t id) const { return id < last; }

public:
    IdTracker() {}

    bool Resize(size_t newCapacity)
    {
        if (recycleBin.Resize(newCapacity))
        {
            capacity = newCapacity;
            if (last > capacity)
                last = capacity;
            return true;
        }
        return false;
    }

    IdTracker(size_t capacity) { Resize(capacity); }

    size_t GetCapacity() { return capacity; }

    bool IsUsed(size_t id) { return InUsedRange(id) && !recycleBin.Get(id); }

    void FreeId(size_t id)
    {
        if (InUsedRange(id))
        {
            if (id == last - 1)
            {
                last--;
                recycleBin.Clear(id);
            }
            else
            {
                recycleBin.Set(id);
            }
        }
    }

    bool AssingId(size_t &id)
    {
        if (last < capacity)
        {
            id = last++;
            return true;
        }

        if (recycleBin.LookupSetPos(id))
        {
            recycleBin.Clear(id);
            return true;
        }

        return false;
    }
};
