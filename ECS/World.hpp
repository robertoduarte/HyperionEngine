#pragma once

#include "EntityReference.hpp"
#include "..\Utils\TemplateUtils.hpp"

namespace Hyperion::ECS
{
    struct World
    {
    private:
        static inline ArchetypeManager* currentManager = nullptr;
        static inline Index currentRow = InvalidIndex;
        
        template <typename T>
        static inline T* currentComponentArray;

    public:
        template <typename Lambda>
        static EntityReference CreateEntity(Lambda lambda)
        {
            using LambdaTraits = LambdaUtil<decltype(&Lambda::operator())>;
            return LambdaTraits::CallWithTypes([&lambda]<typename ...Ts>()
            {
                ArchetypeManager* const manager = ArchetypeManager::Helper<Ts...>::GetInstance();
                const EntityRecord& record = manager->ReserveRecord();
                LambdaTraits::ArgInjectTemplateLambda(lambda, [&manager, &record]<typename T>()
                {
                    return &(manager->template GetComponentArray<T>()[record.row]);
                });
                return EntityReference(record);
            });
        }

        template <typename... Ts>
        static EntityReference CreateEntity()
        {
            ArchetypeManager* const manager = ArchetypeManager::Helper<Ts...>::GetInstance();
            const EntityRecord& record = manager->ReserveRecord();
            ((new (&manager->GetComponentArray<Ts>()[record.row]) Ts()), ...);
            return EntityReference(record);
        }

        template <typename Lambda>
        static void ForEachEntity(Lambda lambda)
        {
            using LambdaTraits = LambdaUtil<decltype(&Lambda::operator())>;
            LambdaTraits::CallWithTypes([&lambda]<typename ...Ts>()
            {
                using LookupCache = ArchetypeManager::LookupCache<Ts...>;
                LookupCache::Update();
                for (size_t i = 0; i < LookupCache::matchCount; i++)
                {
                    currentManager = &ArchetypeManager::managers[LookupCache::matchedIndices[i]];
                    ((currentComponentArray<Ts> = currentManager->template GetComponentArray<Ts>()), ...);
                    for (currentRow = 0; currentRow < currentManager->size; currentRow++)
                    {
                        LambdaTraits::ArgInjectTemplateLambda(lambda, []<typename T>()
                        {
                            return &(currentComponentArray<T>[currentRow]);
                        });
                    }
                    currentRow = InvalidIndex;
                }
            });
        }

        static EntityReference GetCurrentEntity()
        {
            return (currentRow != InvalidIndex) ?
                EntityReference(EntityRecord::records[currentManager->recordIndices[currentRow]]) :
                EntityReference();
        }
    };
}
