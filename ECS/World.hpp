#pragma once

#include "EntityReference.hpp"
#include "..\Utils\TemplateUtils.hpp"

namespace Hyperion::ECS
{
    /**
     * @brief Represents the main world of the ECS (Entity-Component-System).
     */
    struct World
    {
        /**
         * @brief Create a new entity with components using a lambda function.
         * @tparam Lambda The lambda function to initialize entity components.
         * @param lambda The lambda function to initialize the components.
         * @return An EntityReference to the created entity.
         */
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

        /**
         * @brief Create a new entity with specific component types.
         * @tparam Ts The component types to include in the entity.
         * @return An EntityReference to the created entity.
         */
        template <typename... Ts>
        static EntityReference CreateEntity()
        {
            ArchetypeManager* const manager = ArchetypeManager::Helper<Ts...>::GetInstance();
            const EntityRecord& record = manager->ReserveRecord();
            ((new (&manager->GetComponentArray<Ts>()[record.row]) Ts()), ...);
            return EntityReference(record);
        }

        /**
         * @brief Represents an iterator for entities in the ECS world.
         */
        class EntityIterator
        {
            ArchetypeManager* currentManager = nullptr;
            Index currentRow = InvalidIndex;
            bool stop = false;
        public:
            /**
             * @brief Stops the current iteration.
             */
            void StopIteration() { stop = true; };

            /**
             * @brief Get a reference to the current entity in the iteration.
             * @return An EntityReference to the current entity, if available, or an empty one.
             */
            EntityReference GetCurrentEntity()
            {
                return (currentRow != InvalidIndex) ?
                    EntityReference(EntityRecord::records[currentManager->recordIndices[currentRow]]) :
                    EntityReference();
            }

            /**
             * @brief Iterate over entities with specified component types and execute a lambda function.
             * @tparam Lambda The lambda function to execute for each entity.
             * @param lambda The lambda function to execute for each entity, providing access to entity components.
             */
            template <typename Lambda>
            void Iterate(Lambda lambda)
            {
                using LambdaTraits = LambdaUtil<decltype(&Lambda::operator())>;
                LambdaTraits::CallWithTypes([this, lambda]<typename ...Components>()
                {
                    using LookupCache = ArchetypeManager::LookupCache<Components...>;
                    LookupCache::Update();
                    for (size_t i = 0; !stop && i < LookupCache::matchCount; i++)
                    {
                        currentManager = &ArchetypeManager::managers[LookupCache::matchedIndices[i]];
                        [this, lambda](Components* ...componentArray)
                        {
                            for (currentRow = 0; !stop && currentRow < currentManager->size; currentRow++)
                            {
                                lambda(&componentArray[currentRow] ...);
                            }
                        }(currentManager->template GetComponentArray<Components>() ...);
                    }
                });
                currentRow = InvalidIndex;
            }
        };
    };
};
