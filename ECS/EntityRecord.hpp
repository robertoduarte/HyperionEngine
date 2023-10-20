#pragma once

#include <stdint.h>
#include <stddef.h>

#include <stdlib.h>
#include "..\Utils\HierarchicalBitset.hpp"

namespace Hyperion::ECS
{
    using Index = uint16_t;
    static constexpr Index InvalidIndex = ~(Index(0));

    /**
     * @brief Represents a record for an entity in the ECS (Entity-Component-System).
     */
    class EntityRecord
    {
        friend class EntityReference;
        friend class World;
        friend class ArchetypeManager;

        static inline size_t capacity = 0;
        static inline size_t last = 0;
        static inline HierarchicalBitset recycleBin;
        static inline EntityRecord* records = nullptr;

        /**
         * @brief Reserves an entity record, creating a new one or reusing an existing one.
         * @return The reserved EntityRecord.
         */
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

        /**
         * @brief Get the index of the EntityRecord.
         * @return The index of the EntityRecord.
         */
        Index GetIndex() const { return static_cast<Index>(this - records); }

        /**
         * @brief Releases the entity record, making it available for reuse.
         */
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
