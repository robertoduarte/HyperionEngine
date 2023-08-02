#pragma once

#include <limits.h>
#include <stdlib.h>
#include <stdint.h>

#include "..\Utils\std\type_traits.h"

template <typename T>
concept ComponentType = std::is_trivial_v<T> && (!std::is_empty_v<T>);

class Component
{
public:
    using IdType = size_t;

    static inline constexpr size_t MaxComponents = sizeof(IdType) * CHAR_BIT;
private:
    template<size_t N>
    struct reader { friend auto counted_flag(reader<N>); };

    template<size_t N>
    struct setter
    {
        friend auto counted_flag(reader<N>) {}
        static constexpr size_t n = N;
    };

    template< auto Tag, size_t NextVal = 0 >
    [[nodiscard]]
    static consteval auto GetNextID()
    {
        constexpr bool counted_past_value = requires(reader<NextVal> r)
        {
            counted_flag(r);
        };

        if constexpr (counted_past_value)
        {
            return GetNextID<Tag, NextVal + 1>();
        }
        else
        {
            setter<NextVal> s;
            return s.n;
        }
    }

public:
    static inline size_t componentSizes[MaxComponents] = { 0 };

    template <ComponentType T>
    static inline constexpr size_t ID = GetNextID < [] {} > ();

    template <ComponentType T>
        requires(ID<T> < MaxComponents)
    static inline const IdType bitID = []()
    {
        componentSizes[ID<T>] = sizeof(T);
        return (IdType)1 << ID<T>;
    }();


    template <typename... T>
    static IdType CreateID()
    {
        return ((bitID<T>) | ...);
    }

    template <typename... T>
    static IdType AddComponents(IdType sourceID)
    {
        return sourceID | ((bitID<T>) | ...);
    }

    template <typename... T>
    static IdType RemoveComponents(IdType sourceID)
    {
        return sourceID & ~(((IdType)1 << ID<T>) | ...);
    }


    static bool Contains(IdType target, IdType expected)
    {
        return (target & expected) == expected;
    }

    template <typename Lambda>
    static void EachID(IdType id, Lambda lambda)
    {
        size_t componentId = 0;
        do
        {
            if (1 & id)
            {
                lambda(componentId);
            }
            componentId++;
        } while (id >>= 1);
    }

    template <typename Lambda>
    static void EachIdAndSize(IdType id, Lambda lambda)
    {
        size_t componentId = 0;
        do
        {
            if (1 & id)
            {
                lambda(componentId, componentSizes[componentId]);
            }
            componentId++;
        } while (id >>= 1);
    }

    template <typename Lambda>
    static void EachCommonIdAndSize(IdType idA, IdType idB, Lambda lambda)
    {
        size_t componentId = 0;
        do
        {
            if (1 & idA & idB)
            {
                lambda(componentId, componentSizes[componentId]);
            }
            componentId++;
        } while ((idA >>= 1) && (idB >>= 1));
    }
};
