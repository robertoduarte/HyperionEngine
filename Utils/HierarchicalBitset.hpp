#pragma once

#include "SatAlloc.hpp"

#include <limits.h>
#include <stddef.h>

template <size_t capacity>
class FixedHierarchicalBitset;

template <>
class FixedHierarchicalBitset<0>
{
};

template <size_t capacity>
class FixedHierarchicalBitset
{
private:
    static constexpr size_t wordSize = CHAR_BIT * sizeof(size_t);
    static constexpr size_t bitArraySize = (capacity + wordSize - 1) / wordSize;
    size_t bitArray[bitArraySize] = { 0 };

    static constexpr size_t summarySize = (bitArraySize < 2) ? 0 : bitArraySize;
    FixedHierarchicalBitset<summarySize> summary;

    static constexpr bool hasSummary = summarySize != 0;

    static size_t CalculateIndex(size_t pos) { return pos / wordSize; }
    static size_t CalculateBitMask(size_t pos)
    {
        return static_cast<size_t>(1) << (pos % wordSize);
    }

    void SummarySet(size_t index)
    {
        if constexpr (hasSummary)
        {
            summary.Set(index);
        }
    }

    void SummaryClear(size_t index)
    {
        if constexpr (hasSummary)
        {
            if (bitArray[index] == 0)
            {
                summary.Clear(index);
            }
        }
    }

    bool SummaryLookup(size_t& pos) { return false; }

    bool SummaryGet(size_t pos) { return !hasSummary || summary.Get(pos); }

public:
    static constexpr size_t GetCapacity() { return capacity; }

    bool IsValid(size_t pos) const { return pos < capacity; }

    void Clear(size_t pos)
    {
        if (IsValid(pos))
        {
            const size_t index = CalculateIndex(pos);
            const size_t bitMask = CalculateBitMask(pos);
            bitArray[index] &= ~bitMask;
            SummaryClear(index);
        }
    }

    void Set(size_t pos)
    {
        if (IsValid(pos))
        {
            const size_t index = CalculateIndex(pos);
            const size_t bitMask = CalculateBitMask(pos);

            bitArray[index] |= bitMask;
            SummarySet(index);
        }
    }

    bool Get(size_t pos) const
    {
        return IsValid(pos) && SummaryGet(pos) &&
            (bitArray[CalculateIndex(pos)] & CalculateBitMask(pos));
    }

    bool LookupSetPos(size_t& pos) const
    {
        size_t searchIndex = 0;
        if constexpr (hasSummary)
        {
            if (!summary.LookupSetPos(searchIndex))
                return false;
        }

        size_t i = 0;
        size_t searchWord = bitArray[searchIndex];

        while (searchWord && i < wordSize)
        {
            if (searchWord & 1)
            {
                size_t new_pos = (searchIndex * wordSize) + i;
                if (new_pos < capacity)
                {
                    pos = new_pos;
                    return true;
                }
            }

            searchWord >>= 1;
            ++i;
        }

        return false;
    }
};

class HierarchicalBitset
{
private:
    static constexpr size_t wordSize = CHAR_BIT * sizeof(size_t);
    size_t capacity = 0;
    HierarchicalBitset* summary = nullptr;
    size_t* bitArray = nullptr;

    static size_t CalculateIndex(size_t pos) { return pos / wordSize; }
    static size_t CalculateBitMask(size_t pos)
    {
        return static_cast<size_t>(1) << (pos % wordSize);
    }

    void SummaryResize(size_t indexCount)
    {
        if (indexCount > 1)
        {
            if (!summary)
            {
                summary = new HierarchicalBitset(indexCount);

                if (summary)
                {
                    for (size_t i = 0; i < indexCount; i++)
                    {
                        if (bitArray[i] != 0)
                        {
                            summary->Set(i);
                        }
                    }
                }
            }
            else
            {
                summary->Resize(indexCount);
            }
        }
        else
        {
            delete summary;
            summary = nullptr;
        }
    }

    void SummarySet(size_t index)
    {
        if (summary)
        {
            summary->Set(index);
        }
    }

    void SummaryClear(size_t index)
    {
        if (summary && (bitArray[index] == 0))
        {
            summary->Clear(index);
        }
    }

    bool SummaryGet(size_t pos) const { return !summary || summary->Get(pos); }

    static size_t CalculateArraySize(size_t capacity)
    {
        return (capacity + wordSize - 1) / wordSize;
    }

public:
    bool IsValid(size_t pos) const { return pos < capacity; }

    bool Resize(size_t newCapacity)
    {
        const size_t oldBitArraySize = CalculateArraySize(capacity);
        const size_t newBitArraySize = CalculateArraySize(newCapacity);

        if (newBitArraySize != oldBitArraySize)
        {
            size_t* newBitArray = static_cast<size_t*>(
                realloc(bitArray, newBitArraySize * sizeof(size_t)));
            if (!newBitArray)
            {
                return false;
            }
            else
            {
                bitArray = newBitArray;

                for (size_t i = oldBitArraySize; i < newBitArraySize; i++)
                {
                    bitArray[i] = 0;
                }

                SummaryResize(newBitArraySize);
            }
        }
        capacity = newCapacity;
        return true;
    }

    HierarchicalBitset() {}

    HierarchicalBitset(size_t newCapacity) { Resize(newCapacity); }

    ~HierarchicalBitset()
    {
        delete summary;
        free(bitArray);
    }

    size_t GetCapacity() const { return capacity; }

    void Clear(size_t pos)
    {
        if (IsValid(pos))
        {
            const size_t index = CalculateIndex(pos);
            const size_t bitMask = CalculateBitMask(pos);

            bitArray[index] &= ~bitMask;
            SummaryClear(index);
        }
    }

    void Set(size_t pos)
    {
        if (IsValid(pos))
        {
            const size_t index = CalculateIndex(pos);
            const size_t bitMask = CalculateBitMask(pos);

            bitArray[index] |= bitMask;
            SummarySet(index);
        }
    }

    bool Get(size_t pos) const
    {
        return IsValid(pos) && SummaryGet(pos) &&
            (bitArray[CalculateIndex(pos)] & CalculateBitMask(pos));
    }

    bool LookupSetPos(size_t& pos) const
    {
        size_t searchIndex = 0;
        if (summary && !summary->LookupSetPos(searchIndex))
        {
            return false;
        }

        const size_t bitArraySize = CalculateArraySize(capacity);

        size_t i;
        size_t searchWord;

        while (searchIndex < bitArraySize)
        {
            i = 0;
            searchWord = bitArray[searchIndex];

            while (searchWord && i < wordSize)
            {
                if (searchWord & 1)
                {
                    size_t new_pos = (searchIndex * wordSize) + i;
                    if (new_pos < capacity)
                    {
                        pos = new_pos;
                        return true;
                    }
                }
                searchWord >>= 1;
                ++i;
            }
            searchIndex++;
        }

        return false;
    }
};





template <typename T>
struct Sparse8
{
    static constexpr size_t FlagMask = 0b10001000100010001000100010001000;
    
    size_t map = 0;
    T* dense = nullptr;

    size_t GetSize()
    {
        size_t size = 0;
        size_t temp = map;
        while (temp)
        {
            size += 0b1000 & temp;
            temp >>= 4;
        }
        return size >> 3;
    }

    void SetIndex(size_t sparseIndex, size_t denseIndex)
    {
        denseIndex += 8;
        size_t mask = 0b1111;
        while (sparseIndex--)
        {
            denseIndex <<= 4;
            mask <<= 4;
        }
        map = (map & ~mask) | (denseIndex & mask);
    }

    ~Sparse8()
    {
        delete[] dense;
    }

    bool Grow(size_t size)
    {
        if (0b1 & size)
        {
            T* newDense = new T[size + 1];

            if (!newDense) return false;

            for (size_t i = 0; i < size; ++i)
            {
                newDense[i] = std::move(dense[i]);
            }

            delete[] dense;
            dense = newDense;
        }

        return true;
    }

    void Clear(size_t sparseIndex)
    {
        size_t last = 8;
        size_t realPos = 8;

        size_t temp = map;

        for (size_t i = 0; i < 8; i++)
        {
            size_t current = temp & 0b1111;

            if (current > last)
            {
                last = current;
            }

            if (i == sparseIndex)
            {
                realPos = current;
            }

            temp >>= 4;
        }
    }

    T* Get(size_t sparsePos)
    {
        size_t densePosition = map;

        while (sparsePos--) densePosition >>= 4;

        return (0b1000 & densePosition) ? &dense[densePosition & 0b0111] : nullptr;
    }

    T* Set(size_t sparsePos)
    {
        size_t setBitChanger = 0b1000;
        size_t densePosition = map;

        while (sparsePos--)
        {
            setBitChanger <<= 4;
            densePosition >>= 4;
        }

        size_t realPos = densePosition & 0b0111;
        if (0b1000 & (~densePosition))
        {
            if (!Grow(realPos + 1)) return nullptr;

            map ^= setBitChanger;
            map += ((map & FlagMask) ^ FlagMask) >> 3;
        }

        return dense + realPos;
    }
};
