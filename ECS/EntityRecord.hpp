#pragma once

#include <stdint.h>
#include <stddef.h>

#include <stdlib.h>
#include "..\Utils\HierarchicalBitset.hpp"

namespace Hyperion::ECS
{
    using Index = uint16_t;
    static constexpr Index InvalidIndex = ~(Index(0));

    class EntityRecord
    {
        friend class EntityReference;
        friend class World;
        friend class ArchetypeManager;

        static inline size_t capacity = 0;
        static inline size_t last = 0;
        static inline HierarchicalBitset recycleBin;
        static inline EntityRecord* records = nullptr;

        static EntityRecord& Reserve()
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
                size_t originalCapacity = capacity;
                capacity = (capacity == 0) ? 2 : (capacity * 2) - (capacity / 2);
                records = static_cast<EntityRecord*>(realloc(records, sizeof(EntityRecord) * capacity));
                index = last++;

                for (size_t i = originalCapacity; i < capacity; i++)
                {
                    new (&records[index]) EntityRecord();
                }
            }
            return records[index];
        }

        Index archetype = InvalidIndex;
        Index row = InvalidIndex;
        Index version = 0;

        Index GetIndex()const { return static_cast<Index>(this - records); }

        void Release()
        {
            archetype = InvalidIndex;
            row = InvalidIndex;
            version++;

            Index index = GetIndex();
            if (index == last - 1)
            {
                last--;
                recycleBin.Clear(index);
            }
            else
            {
                recycleBin.Set(index);
            }
        }
    };
}
