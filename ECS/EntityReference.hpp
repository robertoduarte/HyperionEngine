#pragma once

#include "Archetype.hpp"

namespace Hyperion::ECS
{
    /**
     * @brief Represents a reference to an entity in the ECS (Entity-Component-System).
     */
    class EntityReference
    {
    private:
        friend class World;

        Index recordIndex = InvalidIndex;
        Index version = InvalidIndex;

        /**
         * @brief Private constructor for creating an EntityReference from an EntityRecord.
         * @param record The EntityRecord to reference.
         */
        EntityReference(const EntityRecord& record) : recordIndex(record.GetIndex()), version(record.version) {}

    public:
        /**
         * @brief Default constructor for creating an empty EntityReference.
         */
        EntityReference() = default;

        /**
         * @brief Access the entity's components and execute a lambda function.
         * @tparam Lambda The lambda function to execute.
         * @param lambda The lambda function to execute, providing access to the entity's components.
         * @return true if the entity is accessible and the lambda executed, false otherwise.
         */
        template <typename Lambda>
        bool Access(Lambda lambda)
        {
            bool status = false;
            if (recordIndex != InvalidIndex)
            {
                const EntityRecord& record = EntityRecord::records[recordIndex];
                if (version == record.version)
                {
                    ArchetypeManager& archetype = ArchetypeManager::managers[record.archetype];
                    using LambdaTraits = LambdaUtil<decltype(&Lambda::operator())>;
                    LambdaTraits::CallWithTypes([lambda, &archetype, &record]<typename ...Components>()
                    {
                        lambda(archetype.GetComponent<Components>(record.row)...);
                    });
                    status = true;
                }
            }
            return status;
        }

        /**
         * @brief Destroy the referenced entity.
         */
        void Destroy()
        {
            if (recordIndex != InvalidIndex)
            {
                const EntityRecord& record = EntityRecord::records[recordIndex];
                recordIndex = InvalidIndex;
                if (version == record.version)
                {
                    ArchetypeManager::managers[record.archetype].RemoveRow(record.row);
                }
            }
        }
    };
}
