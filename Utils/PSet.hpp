#pragma once
#include <stddef.h>
#include <stdint.h>


/**
 * @brief Represents a set of 16 bits.
 */
struct BitSetOf16
{
    uint16_t set = 0; /** The underlying bitset. */

    uint16_t GetMask(uint16_t index)
    {
        uint16_t mask = 1;
        if (index)
        {
            if (index & 1) { mask <<= 1; }
            if (index & 2) { mask <<= 2; }
            if (index & 4) { mask <<= 4; }
            if (index & 8) { mask <<= 8; }
        }
        return mask;
    }

    bool Get(size_t index)
    {
        return set & GetMask(index);
    }

    void Set(size_t index)
    {
        set |= GetMask(index);
    }

    bool IsFull()
    {
        return set == 0xFFFF;
    }

    /**
     * @brief Checks if two BitSetOf16 objects are equal.
     *
     * @param other The other BitSetOf16 object to compare.
     * @return true if the sets are equal, false otherwise.
     */
    bool operator==(const BitSetOf16& other) const { return set == other.set; }

    /**
     * @brief Checks if two BitSetOf16 objects are not equal.
     *
     * @param other The other BitSetOf16 object to compare.
     * @return true if the sets are not equal, false otherwise.
     */
    bool operator!=(const BitSetOf16& other) const { return set != other.set; }

    /**
     * @brief Checks if this BitSetOf16 contains another BitSetOf16.
     *
     * @param other The BitSetOf16 to check for containment.
     * @return true if this set contains the other set, false otherwise.
     */
    bool Contains(const BitSetOf16& other) const { return (set & other.set) == other.set; }

    /**
     * @brief Calls a lambda function for each set bit.
     *
     * @tparam Lambda The type of the lambda function.
     * @param lambda The lambda function to call for each set bit.
     */
    template <typename Lambda>
    void Each(Lambda lambda)
    {
        size_t pos = 0;
        if (IsFull())
        {
            while (pos < 16) { lambda(pos++); }
        }
        else
        {
            uint16_t tempIterator = set;
            do
            {
                if (tempIterator & 1) { lambda(pos++); }
            } while (tempIterator >>= 1);
        }
    }

};


/**
 * @brief Represents a set of 32 bits.
 */
struct BitSetOf32
{
    uint32_t set = 0; /** The underlying bitset. */

    uint32_t GetMask(uint32_t index)
    {
        uint32_t mask = 1;
        if (index)
        {
            if (index & 1) { mask <<= 1; }
            if (index & 2) { mask <<= 2; }
            if (index & 4) { mask <<= 4; }
            if (index & 8) { mask <<= 8; }
            if (index & 16) { mask <<= 16; }
        }
        return mask;
    }

    bool Get(size_t index)
    {
        return set & GetMask(index);
    }

    void Set(size_t index)
    {
        set |= GetMask(index);
    }

    bool IsFull()
    {
        return set == 0xFFFFFFFF;
    }

    /**
     * @brief Checks if two BitSetOf32 objects are equal.
     *
     * @param other The other BitSetOf32 object to compare.
     * @return true if the sets are equal, false otherwise.
     */
    bool operator==(const BitSetOf32& other) const { return set == other.set; }

    /**
     * @brief Checks if two BitSetOf32 objects are not equal.
     *
     * @param other The other BitSetOf32 object to compare.
     * @return true if the sets are not equal, false otherwise.
     */
    bool operator!=(const BitSetOf32& other) const { return set != other.set; }

    /**
     * @brief Checks if this BitSetOf32 contains another BitSetOf32.
     *
     * @param other The BitSetOf32 to check for containment.
     * @return true if this set contains the other set, false otherwise.
     */
    bool Contains(const BitSetOf32& other) const { return (set & other.set) == other.set; }

    /**
     * @brief Calls a lambda function for each set bit.
     *
     * @tparam Lambda The type of the lambda function.
     * @param lambda The lambda function to call for each set bit.
     */
    template <typename Lambda>
    void Each(Lambda lambda)
    {
        size_t pos = 0;
        if (IsFull())
        {
            while (pos < 32) { lambda(pos++); }
        }
        else
        {
            uint32_t tempIterator = set;
            do
            {
                if (tempIterator & 1) { lambda(pos++); }
            } while (tempIterator >>= 1);
        }
    }

};

/**
 * @brief Represents the index of an element within a set.
 */
struct IndexOf64
{
    uint8_t index = 0; /** The index value along with flags. */

    /**
     * @brief Gets the index value.
     *
     * @return The index value.
     */
    auto Get()
    {
        return 0b111111 & index;
    }

    /**
     * @brief Sets the index value.
     *
     * @param srcIndex The index value to set.
     */
    void Set(auto srcIndex)
    {
        index = 0b1000000 | srcIndex;
    }

    /**
     * @brief Checks if the index is free.
     *
     * @return true if the index is free, false otherwise.
     */
    bool IsFree()
    {
        return !index;
    }

    /**
     * @brief Checks if the index is used.
     *
     * @return true if the index is used, false otherwise.
     */
    bool IsUsed()
    {
        return 0b1000000 & index;
    }

    /**
     * @brief Sets the index as full.
     */
    void SetFull()
    {
        index |= 0b10000000;
    }

    /**
     * @brief Checks if the index is full.
     *
     * @return true if the index is full, false otherwise.
     */
    bool IsFull()
    {
        return 0b10000000 & index;
    }

};

/**
 * @brief Represents a sparse set with a dense representation for elements.
 *
 * @tparam T The type of elements stored in the set.
 */
template <typename T>
struct PSet32
{
    IndexOf64 sparse[32]; /** Sparse representation of indices. */
    BitSetOf32 bitSet; /** BitSet for efficiently tracking used indices. */
    T* dense = nullptr; /** Dense representation of elements. */
    uint16_t denseSize = 0; /** Current number of elements in the dense array. */
    uint16_t denseCapacity = 0; /** Capacity of the dense array. */

    /**
     * @brief Reserves memory for the dense array.
     *
     * @param newCapacity The new capacity to reserve.
     * @return true if the reservation was successful, false otherwise.
     */
    bool Reserve(size_t newCapacity)
    {
        if (newCapacity > denseCapacity)
        {
            T* newDense = new T[newCapacity];
            if (!newDense)
            {
                return false;
            }

            for (size_t i = 0; i < denseSize; ++i)
            {
                newDense[i] = std::move(dense[i]);
            }

            delete[] dense;
            dense = newDense;
            denseCapacity = newCapacity;
        }
        return true;
    }

    /**
     * @brief Gets the index object for the specified position.
     *
     * @param pos The position.
     * @return The IndexOf64 object for the position.
     */
    IndexOf64& GetIndex(size_t pos)
    {
        return sparse[pos & 0b11111];
    }

    /**
     * @brief Gets the element at the specified position.
     *
     * @param pos The position.
     * @return A pointer to the element at the specified position, or nullptr if the position is not used.
     */
    T* Get(size_t pos)
    {
        IndexOf64& densePos = GetIndex(pos);
        return densePos.IsUsed() ? &dense[densePos.Get()] : nullptr;
    }

    /**
     * @brief Sets an element at the specified position.
     *
     * @param pos The position.
     * @return A pointer to the element at the specified position, or nullptr if the position is already used.
     */
    T* Set(size_t pos)
    {
        IndexOf64& densePos = GetIndex(pos);
        if (densePos.IsFree())
        {
            if (denseSize == denseCapacity && denseCapacity < 32)
            {
                size_t newCapacity = (denseCapacity == 0) ? 1 : denseCapacity * 2;
                if (!Reserve(newCapacity))
                {
                    return nullptr;
                }
            }
            auto newPos = denseSize++;
            densePos.Set(newPos);
            bitSet.Set(newPos);
        }

        return dense + densePos.Get();
    }

    /**
     * @brief Checks if the BitSetOf32 matches a given lookup.
     *
     * @param lookup The BitSetOf32 to match against.
     * @return true if the bitsets match, false otherwise.
     */
    bool HasMatch(BitSetOf32 lookup)
    {
        return bitSet == lookup;
    }

    /**
     * @brief Checks if the BitSetOf32 is contained in this set.
     *
     * @param lookup The BitSetOf32 to check for containment.
     * @return true if the set contains the lookup, false otherwise.
     */
    bool Contains(BitSetOf32 lookup)
    {
        return bitSet.Contains(lookup);
    }
};

/**
 * @brief Represents a set with a hierarchical structure for handling larger sets.
 *
 * @tparam T The type of elements stored in the set.
 */
template <typename T>
struct PSet1024 : public PSet32<PSet32<T>>
{
    using PSet = PSet32<PSet32<T>>; /** Alias for the base PSet32 type. */

    /**
     * @brief Represents a lookup for efficiently checking matches and containment.
     */
    struct Lookup
    {
        BitSetOf32 base; /** The base BitSetOf32. */
        BitSetOf32 sub[32]; /** Sub BitSets for subsets. */

        /**
         * @brief Sets a bit in the lookup.
         *
         * @param index The index of the bit to set.
         */
        void Set(size_t index)
        {
            size_t pos = 0b11111 & (index >> 5);
            base.Set(pos);
            sub[pos].Set(0b11111 & index);
        }
    };

    /**
     * @brief Sets an element at the specified position.
     *
     * @param pos The position.
     * @return A pointer to the element at the specified position, or nullptr if the position is already used.
     */
    T* Set(size_t pos)
    {
        auto* pSet = PSet::Set(pos >> 5);
        return pSet ? pSet->Set(pos) : nullptr;
    }

    /**
     * @brief Gets the element at the specified position.
     *
     * @param pos The position.
     * @return A pointer to the element at the specified position, or nullptr if the position is not used.
     */
    T* Get(size_t pos)
    {
        auto* pSet = PSet::Get(pos >> 5);
        return pSet ? pSet->Get(pos) : nullptr;
    }

    /**
     * @brief Checks if the lookup matches this set.
     *
     * @param lookup The lookup to match against.
     * @return true if the lookup matches, false otherwise.
     */
    bool HasMatch(Lookup& lookup)
    {
        if (!PSet::HasMatch(lookup.base))
            return false;

        PSet::bitSet.Each([](size_t index)
        {
            if (!PSet::Get(index)->HasMatch(lookup.sub[index]))
                return false;
            return true;
        });

        return true;
    }

    /**
     * @brief Checks if the lookup is contained in this set.
     *
     * @param lookup The lookup to check for containment.
     * @return true if the lookup is contained, false otherwise.
     */
    bool Contains(Lookup& lookup)
    {
        if (!PSet::Contains(lookup.base))
            return false;

        PSet::bitSet.Each([](size_t index)
        {
            if (!PSet::Get(index)->Contains(lookup.sub[index]))
                return false;
            return true;
        });

        return true;
    }
};

struct BitSetOf8
{
    uint8_t set = 0x00;

    uint8_t GetMask(size_t index)
    {
        uint8_t mask = index & 1 ? 2 : 1;
        if (index & 4) { mask <<= 4; }
        return (index & 2) ? mask << 2 : mask;
    }

    bool Get(size_t index)
    {
        return set & GetMask(index);
    }

    void Set(size_t index)
    {
        set |= GetMask(index);
    }

    bool IsFull()
    {
        return set == 0xFF;
    }

    template <typename Lambda>
    void Each(Lambda lambda)
    {
        uint8_t tempIterator = set;
        size_t pos = 0;
        do
        {
            if (tempIterator & 1) { lambda(pos); }
            pos++;
        } while (tempIterator >>= 1);
    }
};

struct UsedOrFull8
{
    uint8_t used = 0x00;
    uint8_t full = 0x00;

    uint8_t GetMask(size_t index)
    {
        uint8_t mask = index & 1 ? 2 : 1;
        if (index & 4) { mask <<= 4; }
        return (index & 2) ? mask << 2 : mask;
    }

    void Set(size_t index, bool setFull = false)
    {
        uint8_t mask = GetMask(index);
        used |= mask;
        if (setFull) full |= mask;
    }

    bool IsFull() { return full == 0xFF; }

    bool Check(size_t index) { return used | GetMask(index); }

    template <typename Lambda>
    void Each(Lambda lambda)
    {
        size_t pos = 0;
        if (IsFull())
        {
            while (pos < 8) { lambda(pos++); }
        }
        else
        {
            uint8_t tempIterator = used;
            do
            {
                if (tempIterator & 1) { lambda(pos++); }
            } while (tempIterator >>= 1);
        }
    }
};


struct UsedOrFull4
{
    uint8_t bitset = 0x00;

    uint8_t MaskFull(size_t index)
    {
        uint8_t mask = index & 1 ? 0b100010 : 0b10001;
        return (index & 2) ? mask << 2 : mask;
    }

    uint8_t MaskUsed(size_t index)
    {
        uint8_t mask = index & 1 ? 0b10 : 0b1;
        return (index & 2) ? mask << 2 : mask;
    }

    bool Check(size_t index) { return bitset & MaskUsed(index); }

    bool IsFull() { return bitset == 0xFF; }

    template <typename Lambda>
    void Each(Lambda lambda, size_t pos = 0)
    {
        if (IsFull())
        {
            size_t end = pos + 4;
            while (pos < end) { lambda(pos++); }
        }
        else
        {
            uint8_t tempIterator = bitset & 0xF;
            do
            {
                if (tempIterator & 1) { lambda(pos++); }
            } while (tempIterator >>= 1);
        }
    }
};

struct SetOf16With4Bit
{
    struct Element
    {
        uint8_t a : 4;
        uint8_t b : 4;
    };

    Element element[8];

    uint8_t Get(size_t index)
    {
        Element& val = element[index >> 1];
        return 1 & index ? val.b : val.a;
    }

    void Set(size_t index, uint8_t value)
    {
        Element& val = element[index >> 1];
        if (1 & index)
            val.b = value;
        else
            val.a = value;
    }
};


struct Sparse16
{
    uint16_t bitset = 0;
    uint8_t sparse[16];

    static uint16_t GetMask(auto index)
    {
        uint16_t mask = 1;
        if (index)
        {
            if (1 & index) { mask <<= 1; }
            if (2 & index) { mask <<= 2; }
            if (4 & index) { mask <<= 4; }
            if (8 & index) { mask <<= 8; }
        }
        return mask;
    }

    template <typename SetLambda, typename InitLambda>
    void Set(SetLambda set, InitLambda init, size_t pos)
    {
        uint16_t mask = GetMask(pos);

        if (!(mask & bitset)) sparse[pos] = init();

        set(sparse[pos]);
    }

    template <typename Lambda>
    void Each(Lambda lambda, size_t pos = 0)
    {
        if (bitset == 0xFFFF)
        {
            size_t end = pos + 16;
            while (pos < end) { lambda(pos++); }
        }
        else
        {
            uint16_t tempIterator = bitset;
            do
            {
                if (tempIterator & 1) { lambda(pos++); }
            } while (tempIterator >>= 1);
        }
    }
};

struct Sparse16FT : public Sparse16
{
    uint16_t fullPositions;
};

template <typename T>
struct Set256 : public Sparse16FT
{
    uint16_t denseSize : 10 = 0;
    uint16_t sparseSetsSize : 6 = 0;
    uint16_t denseCapacity : 10 = 0;
    uint16_t sparseSetsCapacity : 6 = 0;

    Sparse16* sparseSets = nullptr;
    T* dense = nullptr;

    template <typename Lambda>
    void Each(Lambda lambda)
    {
        Sparse16::Each([&lambda](size_t pos)
        {
            sparseSets[pos].Each(lambda, pos * 16);
        });
    }
};

static inline constexpr size_t Sparse16Size = sizeof(Sparse16);
static inline constexpr size_t Sparse16FTSize = sizeof(Sparse16FT);
static inline constexpr size_t testSize = sizeof(Set256<int>);
