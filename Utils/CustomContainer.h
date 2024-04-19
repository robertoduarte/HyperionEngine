#pragma once

#include "std\vector.h"

template <typename... Types>
struct CustomContainer {};

template <typename Head, typename... Tail>
struct CustomContainer<Head, Tail...>
{
private:
    template <typename T, typename... Ts>
    static constexpr bool IsUnique()
    {
        return (!std::is_same_v<T, Ts> && ...);
    }

    template <typename T, typename... Ts>
    static constexpr size_t Order()
    {
        size_t id = 0;
        ((std::is_same_v<T, Ts> ? false : (id++, true)) && ...);
        return id;
    }

public:
    Head value;
    CustomContainer<Tail...> tail;

    template <size_t Index>
    decltype(auto) GetByPosition() const
    {
        if constexpr (Index == 0)
        {
            return value;
        }
        else
        {
            return tail.template GetByPosition<Index - 1>();
        }
    }

    template <typename T>
    decltype(auto) GetByType() const
    {
        static_assert(IsUnique<T, Head, Tail...>, "Type must be unique in CustomContainer to use GetByType");
        constexpr size_t index = Order<T, Head, Tail...>();
        return GetByPosition<index>();
    }
};

template <typename... Types>
CustomContainer<Types...> BuildCustomContainer(Types... args)
{
    return { args... };
}

template <typename Lambda, typename... Types>
void IterateAndApplyLambda(const Lambda& lambda, std::vector<Types>&... containers)
{
    auto vectors = BuildCustomContainer(containers...);

    auto pointers = BuildCustomContainer(containers.begin()...);

    auto end_iterator = iterators.GetByPosition<0>()->end();

    while (iterators.GetByPosition<0>() != end_iterator)
    {
        (lambda(std::get<Types>(iterators)->value), ...);

        // Increment iterators
        (++std::get<Types>(iterators), ...);
    }
}
