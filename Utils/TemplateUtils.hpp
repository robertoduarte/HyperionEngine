#pragma once

#include "std\type_traits.h"

template <size_t...>
struct Sequence
{
};

template <size_t N, size_t... Next>
struct SequenceHelper : public SequenceHelper<N - 1U, N - 1U, Next...>
{
};

template <size_t... Next>
struct SequenceHelper<0U, Next...>
{
    using type = Sequence<Next...>;
};

template <size_t N>
using CreateIndexSequence = typename SequenceHelper<N>::type;

template <size_t... i>
constexpr bool StrCompare(const char *a, const char *b, Sequence<i...>)
{
    return ((a[i] == b[i]) && ...);
}

template <size_t... i>
constexpr size_t FindLastToken(const char *a, const char token, Sequence<i...> indexes)
{
    size_t lastTokenOccurence = indexes.size();
    ((a[i] == token ? (lastTokenOccurence = i, true) : true) && ...);
    return lastTokenOccurence;
}

template <typename T>
constexpr bool MatchesTypeName(const char *str)
{
    constexpr size_t prettyFunctionSize = sizeof(__PRETTY_FUNCTION__);
    constexpr auto begin = FindLastToken(__PRETTY_FUNCTION__, ' ', CreateIndexSequence<prettyFunctionSize>{}) + 1;
    static_assert(begin != prettyFunctionSize, "Failed to get position of type name text.");
    constexpr size_t size = prettyFunctionSize - begin - 1;
    constexpr const char *name = __PRETTY_FUNCTION__ + begin;
    return StrCompare(name, str, CreateIndexSequence<size - 1>{});
}

template <typename T>
inline const char *TypeName()
{
    constexpr size_t prettyFunctionSize = sizeof(__PRETTY_FUNCTION__);
    constexpr size_t begin = FindLastToken(__PRETTY_FUNCTION__, ' ', CreateIndexSequence<prettyFunctionSize>{}) + 1;
    static_assert(begin != prettyFunctionSize, "Failed to get position of type name text.");
    constexpr size_t size = prettyFunctionSize - begin - 1;
    static char typeName[size] = {};
    memcpy(typeName, __PRETTY_FUNCTION__ + begin, size - 1);
    return typeName;
}

template <size_t A, size_t B>
struct PowerOf
{
    static constexpr size_t value = A * PowerOf<A, B - 1>::value;
};

template <size_t A>
struct PowerOf<A, 0>
{
    static constexpr size_t value = 1;
};