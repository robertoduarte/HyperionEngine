#pragma once

#include <stddef.h>
#include <stdint.h>
#include <yaul.h>

/**
 * @brief A class for fixed-point arithmetic operations.
 */
class Fxp
{
private:
    int32_t value; /**< The internal value. */

    /* Division related variables */
    static inline constexpr size_t cpuAddress = 0xFFFFF000UL;
    static inline auto& dvsr = *reinterpret_cast<volatile uint32_t*>(cpuAddress + 0x0F00UL);
    static inline auto& dvdnth = *reinterpret_cast<volatile uint32_t*>(cpuAddress + 0x0F10UL);
    static inline auto& dvdntl = *reinterpret_cast<volatile uint32_t*>(cpuAddress + 0x0F14UL);

    friend class Vec3;
    friend class Trigonometry;

    struct Internal
    {
        int16_t high;
        uint16_t low;
        constexpr Internal(const int16_t& highVal, const uint16_t& lowVal = 0) : high(highVal), low(lowVal) {}
        constexpr Fxp& AsFxp() { return *reinterpret_cast<Fxp*>(this); }
    };

    constexpr Internal& InternalAccess() { return *reinterpret_cast<Internal*>(value); };

    /**
     * @brief Private constructor for creating Fxp objects from an int32_t value.
     * @param inValue The int32_t value to store.
     */
    constexpr Fxp(const int32_t& inValue) : value(inValue) {}

public:
    /**
     * @brief Default constructor, initializes the value to 0.
     */
    constexpr Fxp() : value(0) {}

    /**
     * @brief Copy constructor.
     * @param fxp The Fxp object to copy.
     */
    constexpr Fxp(const Fxp& fxp) : value(fxp.value) {}

    /**
     * @brief Constructor to create an Fxp object from a floating-point value.
     * @param f The floating-point value to convert to fixed-point.
     */
    consteval Fxp(const double& f) : value(f * 65536.0) {}

    /***********Static Functions************/

    /**
     * @brief Determine the maximum of two Fxp objects.
     * @param a The first Fxp object.
     * @param b The second Fxp object.
     * @return The maximum of the two Fxp objects.
     */
    static constexpr Fxp Max(const Fxp& a, const Fxp& b)
    {
        return (a > b) ? a : b;
    }

    /**
     * @brief Determine the minimum of two Fxp objects.
     * @param a The first Fxp object.
     * @param b The second Fxp object.
     * @return The minimum of the two Fxp objects.
     */
    static constexpr Fxp Min(const Fxp& a, const Fxp& b)
    {
        return (a < b) ? a : b;
    }

    /**
     * @brief Create an Fxp object from a 16-bit integer value.
     * @param integerValue The 16-bit integer value.
     * @return The corresponding Fxp object.
     */
    static constexpr Fxp FromInt(const int16_t& integerValue) { return Internal(integerValue).AsFxp(); }

    /**
     * @brief Build an Fxp object from a raw 32-bit integer value.
     * @param rawValue The raw 32-bit integer value.
     * @return The corresponding Fxp object.
     */
    static constexpr Fxp BuildRaw(const int32_t& rawValue) { return Fxp(rawValue); }


    /**
     * @brief Set the dividend and divisor for asynchronous fixed-point division.
     * @param dividend The dividend.
     * @param divisor The divisor.
     */
    static void AsyncDivSet(const Fxp& dividend, const Fxp& divisor)
    {
        uint32_t dividendh;
        __asm__ volatile("swap.w %[in], %[out]\n"
            : [out] "=&r"(dividendh)
            : [in] "r"(dividend.value));
        __asm__ volatile("exts.w %[in], %[out]"
            : [out] "=&r"(dividendh)
            : [in] "r"(dividendh));

        dvdnth = dividendh;
        dvsr = divisor.value;
        dvdntl = dividend.value << 16;
    }

    /**
     * @brief Get the result of asynchronous fixed-point division.
     * @return The quotient.
     */
    static Fxp AsyncDivGet() { return static_cast<int32_t>(dvdntl); }

    /***********Member Functions************/

    /**
     * @brief Truncate the fractional part of the value.
     * @return The truncated Fxp object.
     */
    constexpr Fxp TruncateFraction() const
    {
        return (int32_t)0xFFFF0000 & value;
    }

    /**
     * @brief Calculate the square root of the value.
     * @return The square root as an Fxp object.
     */
    constexpr Fxp Sqrt() const
    {
        uint32_t remainder = static_cast<uint32_t>(value);
        uint32_t root = 0;
        uint32_t bit = 0x40000000; // Set the initial bit to the highest bit for 32-bit integers

        while (bit > 0x40)
        {
            // Try the current bit in the root calculation
            uint32_t trial = root + bit;

            if (remainder >= trial)
            {
                remainder -= trial;
                root = trial + bit;
            }

            // Shift the remainder and decrease the bit for the next iteration
            remainder <<= 1;
            bit >>= 1;
        }

        root >>= 8; // Adjust the result

        return static_cast<int32_t>(root);
    }

    /**
     * @brief Faster and slightly less accurate version of Sqrt.
     * Due to interpolation, it has a maximum error of around 6.08%.
     * @return The square root as an Fxp object.
     */
    constexpr Fxp FastSqrt() const
    {
        int32_t baseEstimation = 0;
        int32_t estimation = value;

        if (estimation > 0)
        {
            if (estimation < 65536)
            {
                baseEstimation = 1 << 7;
                estimation <<= 7;

                uint32_t iterationValue = value >> 1;
                while (iterationValue)
                {
                    estimation >>= 1;
                    baseEstimation <<= 1;
                    iterationValue >>= 2;
                }
            }
            else
            {
                baseEstimation = (1 << 14);

                while (baseEstimation < estimation)
                {
                    estimation >>= 1;
                    baseEstimation <<= 1;
                }
            }
        }

        return baseEstimation + estimation;
    }

    /**
     * @brief Calculate the square of the value.
     * @return The square as an Fxp object.
     */
    constexpr Fxp Square() const { return *this * *this; }

    /**
     * @brief Calculate the absolute value of the object.
     * @return The absolute value as an Fxp object.
     */
    constexpr Fxp Abs() const { return value > 0 ? value : -value; }

    /**
     * @brief Get the internal value.
     * @return A reference to the internal value.
     */
    constexpr const int32_t& Value() const { return value; }

    /**
     * @brief Convert the fixed-point value to a floating-point value.
     * @return The floating-point representation of the value.
     */
    consteval double ToFloat() { return value / 65536.0f; }

    /**
     * @brief Convert the fixed-point value to a 16-bit integer value.
     * @return The 16-bit integer representation of the value.
     */
    constexpr int16_t ToInt() { return InternalAccess().high; }

    /**************Operators****************/

    /**
     * @brief Add and assign another Fxp object to this object.
     * @param fxp The Fxp object to add.
     * @return A reference to this object.
     */
    constexpr Fxp& operator+=(const Fxp& fxp) { value += fxp.value; return *this; }

    /**
     * @brief Subtract and assign another Fxp object from this object.
     * @param fxp The Fxp object to subtract.
     * @return A reference to this object.
     */
    constexpr Fxp& operator-=(const Fxp& fxp) { value -= fxp.value; return *this; }

    /**
     * @brief Multiply and assign another Fxp object to this object.
     * @param fxp The Fxp object to multiply.
     * @return A reference to this object.
     */
    constexpr Fxp& operator*=(const Fxp& fxp)
    {
        if consteval
        {
            double a = value / 65536.0;
            double b = fxp.value / 65536.0;
            value = (a * b) * 65536.0;;
        }
        else
        {
            uint32_t mach;
            __asm__ volatile(
                "\tdmuls.l %[a], %[b]\n"
                "\tsts mach, %[mach]\n"
                "\tsts macl, %[out]\n"
                "\txtrct %[mach], %[out]\n"
                /* Output */
                : [mach] "=&r" (mach),
                [out] "=&r" (value)
                /* Input */
                : [a] "r" (value),
                [b] "r" (fxp.value)
                : "mach", "macl");
        }

        return *this;
    }

    /**
     * @brief Multiply two Fxp objects.
     * @param fxp The multiplier.
     * @return The product as an Fxp object.
     */
    constexpr Fxp operator*(const Fxp& fxp) const
    {
        return Fxp(*this) *= fxp;
    }

    /**
     * @brief Divide and assign another Fxp object to this object.
     * @param fxp The Fxp object to divide by.
     * @return A reference to this object.
     */
    constexpr Fxp& operator/=(const Fxp& fxp)
    {
        if consteval
        {
            double a = value / 65536.0;
            double b = fxp.value / 65536.0;
            value = (a / b) * 65536.0;;
        }
        else
        {
            AsyncDivSet(*this, fxp);
            value = static_cast<int32_t>(dvdntl);
        }
        return *this;
    }

    /**
     * @brief Divide two Fxp objects.
     * @param fxp The divisor.
     * @return The quotient as an Fxp object.
     */
    constexpr Fxp operator/(const Fxp& fxp) const
    {
        return Fxp(*this) /= fxp;
    }

    /**
     * @brief Copy assignment operator.
     * @param fxp The Fxp object to copy.
     * @return A reference to this object.
     */
    constexpr Fxp& operator=(const Fxp&) = default;

    /**
     * @brief Negate the value.
     * @return The negated value as an Fxp object.
     */
    constexpr Fxp operator-() const { return -value; }

    /**
     * @brief Add another Fxp object to this object.
     * @param fxp The Fxp object to add.
     * @return The sum as an Fxp object.
     */
    constexpr Fxp operator+(const Fxp& fxp) const { return value + fxp.value; }

    /**
     * @brief Subtract another Fxp object from this object.
     * @param fxp The Fxp object to subtract.
     * @return The difference as an Fxp object.
     */
    constexpr Fxp operator-(const Fxp& fxp) const { return value - fxp.value; }

    /**
     * @brief Compare two Fxp objects for greater than.
     * @param fxp The Fxp object to compare with.
     * @return `true` if this object is greater than the other; otherwise, `false`.
     */
    constexpr bool operator>(const Fxp& fxp) const { return value > fxp.value; }

    /**
     * @brief Compare two Fxp objects for greater than or equal.
     * @param fxp The Fxp object to compare with.
     * @return `true` if this object is greater than or equal to the other; otherwise, `false`.
     */
    constexpr bool operator>=(const Fxp& fxp) const { return value >= fxp.value; }

    /**
     * @brief Compare two Fxp objects for less than.
     * @param fxp The Fxp object to compare with.
     * @return `true` if this object is less than the other; otherwise, `false`.
     */
    constexpr bool operator<(const Fxp& fxp) const { return value < fxp.value; }

    /**
     * @brief Compare two Fxp objects for less than or equal.
     * @param fxp The Fxp object to compare with.
     * @return `true` if this object is less than or equal the other; otherwise, `false`.
     */
    constexpr bool operator<=(const Fxp& fxp) const { return value <= fxp.value; }

    /**
     * @brief Compare two Fxp objects for equality.
     * @param fxp The Fxp object to compare with.
     * @return `true` if this object is equal to the other; otherwise, `false`.
     */
    constexpr bool operator==(const Fxp& fxp) const { return value == fxp.value; }

    /**
     * @brief Compare two Fxp objects for inequality.
     * @param fxp The Fxp object to compare with.
     * @return `true` if this object is not equal to the other; otherwise, `false`.
     */
    constexpr bool operator!=(const Fxp& fxp) const { return value != fxp.value; }

    /**
     * @brief Right shift operator for logical right shift.
     * @param shiftAmount The number of bits to shift.
     * @return The result of the logical right shift as an Fxp object.
     */
    constexpr Fxp operator>>(const size_t& shiftAmount) const { return value >> shiftAmount; }

    /**
     * @brief Right shift and assign operator for logical right shift.
     * @param shiftAmount The number of bits to shift.
     * @return A reference to this object after the logical right shift.
     */
    constexpr Fxp& operator>>=(const size_t& shiftAmount) { value >>= shiftAmount; return *this; }

    /**
     * @brief Left shift operator for shifting the internal value by a specified number of bits.
     * @param shiftAmount The number of bits to shift the internal value to the left.
     * @return A new Fxp object with the internal value left-shifted by the specified amount.
     */
    constexpr Fxp operator<<(const size_t& shiftAmount) const { return value << shiftAmount; }

    /**
     * @brief In-place left shift operator for shifting the internal value by a specified number of bits.
     * @param shiftAmount The number of bits to shift the internal value to the left.
     * @return A reference to this Fxp object after left-shifting the internal value in place.
     */
    constexpr Fxp& operator<<=(const size_t& shiftAmount) { value <<= shiftAmount; return *this; }
};
