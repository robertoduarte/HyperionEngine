#pragma once

#include <limits.h>
#include "..\Utils\std\type_traits.h"
#include "..\Utils\TemplateUtils.hpp"

template <typename T>
concept ComponentType = (!std::is_empty_v<T>);

template <ComponentType... SupportedTypes>
class ArchetypeId
{
private:
    static constexpr size_t arraySize = (sizeof...(SupportedTypes) + CHAR_BIT - 1) / CHAR_BIT;
    uint8_t array[arraySize] = {0};

    constexpr size_t GetIndex(size_t pos) const { return pos / CHAR_BIT; }
    constexpr uint8_t GetBitMask(size_t pos) const { return 1 << pos % CHAR_BIT; }

    template <size_t... i>
    constexpr bool ContainsArchetype(const ArchetypeId &a, Sequence<i...>) const { return (((array[i] & a.array[i]) == array[i]) && ...); }

    template <size_t... i>
    constexpr bool Equals(const ArchetypeId &a, Sequence<i...>) const { return ((array[i] == a.array[i]) && ...); }

    template <size_t... i>
    constexpr bool IsEmpty(Sequence<i...> index) const { return ((array[i] == 0) && ...); }

    template <size_t... i>
    constexpr bool GeaterThan(const ArchetypeId &a, Sequence<i...>) const { return (... && (array[i] > a.array[i])); }

    template <size_t... i>
    constexpr bool SmallerThan(const ArchetypeId &a, Sequence<i...>) const { return (... && (array[i] < a.array[i])); }

    template <size_t... i>
    constexpr void Copy(const ArchetypeId &a, Sequence<i...>) { ((array[i] = a.array[i]), ...); }

    template <size_t... i>
    constexpr void Add(const ArchetypeId &a, Sequence<i...>) { ((array[i] |= a.array[i]), ...); }

    template <size_t... i>
    constexpr void Subtract(const ArchetypeId &a, Sequence<i...>) { ((array[i] &= ~a.array[i]), ...); }

    template <typename T>
    static constexpr size_t Order()
    {
        size_t index = 0;
        ((std::is_same_v<T, SupportedTypes> ? false : (index++, true)) && ...);
        return index;
    }

    static constexpr size_t ComponentIndexFromName(const char *name)
    {
        size_t index = 0;
        ((MatchesTypeName<SupportedTypes>(name) ? (index = ComponentIndex<SupportedTypes>(), false) : true) && ...);
        return index;
    }

    template <size_t pos>
    constexpr void Set()
    {
        array[GetIndex(pos)] |= GetBitMask(pos);
    }

    template <size_t pos>
    constexpr bool Check() const
    {
        return array[GetIndex(pos)] & GetBitMask(pos);
    }

public:
    // Type Utils
    static constexpr size_t ComponentCount()
    {
        return sizeof...(SupportedTypes);
    }

    template <typename Type>
    static constexpr size_t ComponentIndex()
    {
        static_assert((... || std::is_same_v<Type, SupportedTypes>),
                      "Type not supported by ArchetypeId");

        size_t biggerCnt = ((!std::is_same_v<Type, SupportedTypes> && sizeof(SupportedTypes) > sizeof(Type)) + ...);
        size_t sameSizeCnt = ((Order<SupportedTypes>() < Order<Type>() && sizeof(SupportedTypes) == sizeof(Type)) + ...);

        return biggerCnt + sameSizeCnt;
    }

    template <typename... T>
    static consteval auto BuildArchetypeId()
    {
        ArchetypeId a;
        if constexpr (sizeof...(T))
        {
            ((a.template Set<ComponentIndex<T>()>()), ...);
        }
        return a;
    }

    // Constructors
    constexpr ArchetypeId(const char *typeName) { Set<ComponentIndexFromName(typeName)>(); };
    constexpr ArchetypeId(const ArchetypeId &a) { Copy(a, CreateIndexSequence<arraySize>{}); };
    constexpr ArchetypeId(){};

    // Functions
    constexpr size_t Size() const
    {
        return ((Contains<SupportedTypes>() ? sizeof(SupportedTypes) : 0) + ...);
    }

    constexpr bool Contains(const ArchetypeId &a) const { return ContainsArchetype(a, CreateIndexSequence<arraySize>{}); }

    template <typename T>
    constexpr bool Contains() const { return Check<ComponentIndex<T>()>(); }

    constexpr bool IsEmpty() const { return IsEmpty(CreateIndexSequence<arraySize>{}); }

    template <typename T>
    constexpr size_t GetOffset() const
    {
        return ((ComponentIndex<SupportedTypes>() < ComponentIndex<T>() && Contains<SupportedTypes>() ? sizeof(SupportedTypes) : 0) + ...);
    }

    // Operators
    constexpr bool operator==(const ArchetypeId &a) const { return Equals(a, CreateIndexSequence<arraySize>{}); }
    constexpr bool operator>(const ArchetypeId &a) const { return GeaterThan(a, CreateIndexSequence<arraySize>{}); }
    constexpr bool operator<(const ArchetypeId &a) const { return SmallerThan(a, CreateIndexSequence<arraySize>{}); }

    constexpr ArchetypeId &operator+=(const ArchetypeId &a)
    {
        Add(a, CreateIndexSequence<arraySize>{});
        return *this;
    }

    constexpr ArchetypeId &operator-=(const ArchetypeId &a)
    {
        Subtract(a, CreateIndexSequence<arraySize>{});
        return *this;
    }

    constexpr ArchetypeId operator+(const ArchetypeId &a) const { return ArchetypeId(*this) += a; }
    constexpr ArchetypeId operator-(const ArchetypeId &a) const { return ArchetypeId(*this) -= a; }
};
