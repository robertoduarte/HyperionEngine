#pragma once

#include <limits.h>
#include <stddef.h>
#include "ComponentInfo.hpp"

struct ArchetypeId
{
    friend class ArchetypeMgr;
    friend class Entity;

    using idType = size_t;
    static constexpr size_t maxComponents = sizeof(idType) * CHAR_BIT;

    idType id = 0;

    size_t GetSizeAndSetOffsets(size_t *offsetArray = nullptr) const
    {
        size_t adder = 0;
        size_t index = 0;
        idType temp = id;
        while (temp)
        {
            if (temp & 1)
            {
                if (offsetArray)
                    offsetArray[index] = adder;
                adder += Component::SizeFromId(index);
            }
            index++;
            temp >>= 1;
        }
        return adder;
    }

    static idType GetMask(size_t componentId) { return ((idType)1) << componentId; }

    bool Contains(size_t componentId) const { return id & GetMask(componentId); }

    ArchetypeId(const ArchetypeId &a) : id(a.id){};
    ArchetypeId() : id(0){};

    template <typename... T>
    static ArchetypeId BuildArchetypeId()
    {
        ArchetypeId a;
        ((a.id |= GetMask(Component::Id<T>)), ...);
        return a;
    }

    bool Contains(const ArchetypeId &a) const { return (id & a.id) == a.id; }

    bool IsEmpty() const { return id == 0; }

    template <typename T>
    bool Contains() const { return Contains(Component::Id<T>); }

    // Operators
    ArchetypeId &operator+=(const ArchetypeId &a)
    {
        id |= a.id;
        return *this;
    }

    ArchetypeId &operator-=(const ArchetypeId &a)
    {
        id &= ~a.id;
        return *this;
    }

    ArchetypeId operator+(const ArchetypeId &a) const { return ArchetypeId(*this) += a; }
    ArchetypeId operator-(const ArchetypeId &a) const { return ArchetypeId(*this) -= a; }
    bool operator==(const ArchetypeId &a) const { return id == a.id; }
    bool operator!=(const ArchetypeId &a) const { return id != a.id; }
    bool operator>(const ArchetypeId &a) const { return id > a.id; }
    bool operator<(const ArchetypeId &a) const { return id < a.id; }
};