#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "..\Utils\StringUtils.hpp"

#include <cpu/divu.h>
#include <fix16.h>

class Fxp
{
private:
    int32_t value;

    friend class Fxp3D;

    static constexpr size_t fractionMax = 100000; // Fraction max is an whole number (1.0)
    static constexpr size_t fractionBits = 16;    // Fraction side of the Q15.16 fixed point format
    static constexpr uint16_t base2Remainders[fractionBits] =
        {fractionMax / (1 << 1),
         fractionMax / (1 << 2),
         fractionMax / (1 << 3),
         fractionMax / (1 << 4),
         fractionMax / (1 << 5),
         fractionMax / (1 << 6),
         fractionMax / (1 << 7),
         fractionMax / (1 << 8),
         fractionMax / (1 << 9),
         fractionMax / (1 << 10),
         fractionMax / (1 << 11),
         fractionMax / (1 << 12),
         fractionMax / (1 << 13),
         fractionMax / (1 << 14),
         fractionMax / (1 << 15),
         fractionMax / (1 << 16)};

    static int32_t ConvertToBase2(int32_t srcBase10)
    {
        int32_t base2Result = 0;
        int32_t bitAdder = 1 << (fractionBits - 1);

        for (size_t i = 0; i < fractionBits; i++)
        {
            if (srcBase10 - base2Remainders[i] > 0)
            {
                srcBase10 -= base2Remainders[i];
                base2Result += bitAdder;
            }
            bitAdder >>= 1;
        }
        return base2Result;
    }

    constexpr Fxp(const int32_t &value) : value(value) {}

public:
    constexpr Fxp() : value(0) {}

    constexpr Fxp(const Fxp &fxp) : value(fxp.value) {}

    constexpr Fxp(const float &f) : value(f * 65536.0) {}

    Fxp(const char *str) { Parse(str); }

    size_t Parse(const char *str)
    {
        const char *tmpStr = str;

        tmpStr += StringUtils::Parse(value, tmpStr);
        value <<= fractionBits;

        if (StringUtils::SkipUntil(&tmpStr, '.'))
        {
            tmpStr++;
            char paddedFraction[] = "00000";
            char *fractionStr = paddedFraction;
            while (*fractionStr != '\0' && *tmpStr >= '0' && *tmpStr <= '9')
                *(fractionStr++) = *(tmpStr++);

            int32_t fractionBase10 = 0;
            StringUtils::Parse(fractionBase10, paddedFraction);
            int32_t fxpFraction = ConvertToBase2(fractionBase10);

            value += (value < 0) ? -fxpFraction : fxpFraction;
        }

        return tmpStr - str;
    }

    /***********Static Functions************/

    static void AsyncDivSet(Fxp dividend, Fxp divisor)
    {
        cpu_divu_fix16_set(dividend.value, divisor.value);
    }
    static Fxp AsyncDivGet() { return (int32_t)cpu_divu_quotient_get(); }

    /***********Member Functions************/

    Fxp Sqrt() const { return fix16_sqrt(value); }
    Fxp Square() const { return *this * *this; }
    Fxp Abs() const { return fix16_abs(value); }
    Fxp Tan() const { return fix16_tan(value); }
    const int32_t &Value() { return value; }

    /**************Operators****************/

    Fxp &operator+=(Fxp fxp)
    {
        value += fxp.value;
        return *this;
    }

    Fxp &operator-=(Fxp fxp)
    {
        value -= fxp.value;
        return *this;
    }

    Fxp &operator*=(Fxp fxp)
    {
        value = fix16_mul(value, fxp.value);
        return *this;
    }

    Fxp &operator/=(Fxp fxp)
    {
        cpu_divu_fix16_set(value, fxp.value);
        value = (int32_t)cpu_divu_quotient_get();
        return *this;
    }

    Fxp operator/(Fxp fxp) const
    {
        cpu_divu_fix16_set(value, fxp.value);
        return (int32_t)cpu_divu_quotient_get();
    }

    Fxp operator*(Fxp fxp) const { return fix16_mul(value, fxp.value); }
    constexpr Fxp operator-() const { return -value; }
    constexpr Fxp operator+(Fxp fxp) const { return value + fxp.value; }
    constexpr Fxp operator-(Fxp fxp) const { return value - fxp.value; }

    constexpr bool operator>(Fxp fxp) const { return value > fxp.value; }
    constexpr bool operator>=(Fxp fxp) const { return value >= fxp.value; }
    constexpr bool operator<(Fxp fxp) const { return value < fxp.value; }
    constexpr bool operator==(Fxp fxp) const { return value == fxp.value; }
    constexpr bool operator!=(Fxp fxp) const { return value != fxp.value; }
};
