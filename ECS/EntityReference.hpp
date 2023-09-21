#pragma once

#include "Archetype.hpp"

namespace Hyperion::ECS
{
    class EntityReference
    {
    private:
        friend class World;

        Index recordIndex = InvalidIndex;
        Index version = InvalidIndex;

        EntityReference(const EntityRecord& record) : recordIndex(record.GetIndex()), version(record.version) {}

    public:
        EntityReference() = default;

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
                    LambdaTraits::ArgInjectTemplateLambda(lambda, [&archetype, &record]<typename T>()
                    {
                        return archetype.GetComponent<T>(record.row);
                    });
                    status = true;
                }
            }
            return status;
        }

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
