#pragma once

#include <stdint.h>
#include <stddef.h>

#include <stdlib.h>
#include "..\Utils\HierarchicalBitset.hpp"

#include "Component.hpp"
#include "Archetype.hpp"

using Index = uint16_t;
static constexpr Index InvalidIndex = ~(Index(0));

class Entity
{
private:
    friend class ArchetypeManager;
    friend class World;

    struct Record
    {
        static inline Index capacity = 0;
        static inline Index last = 0;
        static inline HierarchicalBitset recycleBin;
        static inline Record* records = nullptr;

        static Record& Reserve()
        {
            size_t index;
            if (last < capacity)
            {
                index = last++;
            }
            else if (recycleBin.LookupSetPos(index))
            {
                recycleBin.Clear(index);
            }
            else
            {
                capacity = (capacity == 0) ? 2 : capacity * 2;
                records = static_cast<Record*>(realloc(records, sizeof(Record) * capacity));
                index = last++;
            }
            return records[index];
        }

        Index archetype = InvalidIndex;
        Index row = InvalidIndex;
        Index referenceCounter = 0;

        Index GetIndex() { return static_cast<Index>(this - records); }

        void TrySelfDestruct()
        {
            if (referenceCounter != 0 || archetype != InvalidIndex)
            {
                return;
            }

            Index thisIndex = static_cast<Index>(this - records);
            if (thisIndex == last - 1)
            {
                last--;
                recycleBin.Clear(thisIndex);
            }
            else
            {
                recycleBin.Set(thisIndex);
            }
        }

        void Invalidate() { archetype = InvalidIndex; TrySelfDestruct(); }
        void IncreaseCounter() { referenceCounter++; }
        void DecreaseCounter() { referenceCounter--; }
    };

    Index recordIndex = InvalidIndex;

    template <typename Lambda>
    void AccessRecord(Lambda lambda)
    {
        if (recordIndex != InvalidIndex)
        {
            lambda((Record&)Record::records[recordIndex]);
        }
    }

    void Clear()
    {
        AccessRecord([](Record& record)
        {
            record.DecreaseCounter();
            record.TrySelfDestruct();
        });
    }

    explicit Entity(Index recordIndex) : recordIndex(recordIndex)
    {
        AccessRecord([](Record& record) { record.IncreaseCounter(); });
    }


public:
    Entity() = default;

    ~Entity() { Clear(); }

    Entity(const Entity& other) : recordIndex(other.recordIndex)
    {
        AccessRecord([](Record& record) { record.IncreaseCounter(); });
    }

    Entity& operator=(const Entity& other)
    {
        if (this != &other)
        {
            Clear();
            recordIndex = other.recordIndex;
            AccessRecord([](Record& record) { record.IncreaseCounter(); });
        }
        return *this;
    }

    Entity(Entity&& other) noexcept : recordIndex(other.recordIndex)
    {
        other.recordIndex = InvalidIndex;
    }

    Entity& operator=(Entity&& other) noexcept
    {
        if (this != &other)
        {
            Clear();
            recordIndex = other.recordIndex;
            other.recordIndex = InvalidIndex;
        }
        return *this;
    }
};
