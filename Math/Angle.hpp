#pragma once

#include "Fxp.hpp"

class Angle
{
private:
    uint16_t value;

    struct TrigMetaData
    {
        Fxp preCalculation;
        uint32_t interpolation;
    };

    static constexpr TrigMetaData sinLut[] = {{0.000000f, 205556},
                                              {0.098017f, 203577},
                                              {0.195090f, 199637},
                                              {0.290285f, 193774},
                                              {0.382683f, 186045},
                                              {0.471397f, 176524},
                                              {0.555570f, 165303},
                                              {0.634393f, 152491},
                                              {0.707107f, 138210},
                                              {0.773010f, 122597},
                                              {0.831470f, 105804},
                                              {0.881921f, 87992},
                                              {0.923880f, 69333},
                                              {0.956940f, 50006},
                                              {0.980785f, 30197},
                                              {0.995185f, 10098},
                                              {1.000000f, 0}};

    static constexpr Fxp SinInternal(uint16_t angle)
    {
        bool secondHalf = angle > 32767;
        angle += ((-secondHalf) & 32768);

        // If it is in the second quarter invert the index
        angle -= (-(angle > 16383)) & ((angle - 16384) * 2);

        // Extract index
        size_t index = angle >> 10;

        // Extract Multiplier
        int32_t interpolationMultiplier = angle & 0x3FF;

        // Calculate interpolation with scaled 32 bit integer multiplication to reduce rounding errors
        int32_t interpolation = (interpolationMultiplier * sinLut[index].interpolation) >> 15;

        int32_t ret = sinLut[index].preCalculation.value + interpolation;

        return ((-(!secondHalf)) & ret) + ((-(secondHalf)) & -ret);
    }

    struct TanLutBlock
    {
        uint16_t indexSubtractor;
        uint16_t indexShift;
        uint16_t interpolationScaleShift;
        uint16_t multiplierMask;
        TrigMetaData lut[5];
    };

    static constexpr TanLutBlock tanLut[] =
        {{0x0000, 12, 9, 0x0FFF, {{0.0f, 3393}, {0.41421f, 4798}, {1.00000f, 11585}, {0.0f, 0}, {0.0f, 0}}},
         {0x3000, 10, 8, 0x03FF, {{2.41421f, 14456}, {3.29656f, 28357}, {5.02734f, 83981}, {0.0f, 0}, {0.0f, 0}}},
         {0x3C00, 8, 0, 0x00FF, {{10.15317f, 871}, {13.55667f, 1740}, {20.35547f, 5217}, {0.0f, 0}, {0.0f, 0}}},
         {0x3F00, 6, 0, 0x003F, {{40.73548f, 13909}, {54.31875f, 27816}, {81.48324f, 83445}, {0.0f, 0}, {0.0f, 0}}},
         {0x3FC0, 4, 0, 0x000F, {{162.97262f, 222516}, {217.29801f, 445031}, {325.94830f, 1335090}, {0.0f, 0}, {0.0f, 0}}},
         {0x3FF0, 2, 0, 0x0003, {{651.89814f, 3560237}, {869.19781f, 7120473}, {1303.79704f, 21361417}, {2607.59446f, 494148084}, {(int32_t)2147483647, 0}}}};

    constexpr Angle(const uint16_t &value) : value(value) {}

public:
    constexpr Angle() : value(0) {}

    static consteval Angle DegreesToAngle(const float &degrees)
    {
        return TurnsToAngle(degrees / 360.0F);
    }

    static constexpr Angle TurnsToAngle(Fxp turns)
    {
        return turns.value < 0 ? -turns.value : turns.value;
    }
    
    constexpr Fxp Sin() const
    {
        return SinInternal(value);
    }

    constexpr Fxp Cos() const
    {
        return SinInternal(value + 16384);
    }

    constexpr Fxp Tan() const
    {
        uint16_t temp = value + ((-(value > 32767)) & 32768);
        bool secondQuarter = value > 16384;
        // If it is in the second quarter invert the index
        temp -= (-secondQuarter) & ((temp - 16384) * 2);

        // Determine table index
        size_t lutBlock = (temp >= 0x3000) +
                          (temp >= 0x3C00) +
                          (temp >= 0x3F00) +
                          (temp >= 0x3FC0) +
                          (temp >= 0x3FF0);

        // Extract index
        size_t index = (temp - tanLut[lutBlock].indexSubtractor) >> tanLut[lutBlock].indexShift;

        // Extract Multiplier
        int32_t interpolationMultiplier = temp & tanLut[lutBlock].multiplierMask;

        // Calculate interpolation with scaled 32 bit integer multiplication to reduce rounding errors
        int32_t interpolation = (interpolationMultiplier * tanLut[lutBlock].lut[index].interpolation) >> tanLut[lutBlock].interpolationScaleShift;

        int32_t ret = tanLut[lutBlock].lut[index].preCalculation.value + interpolation;

        return ((-(!secondQuarter)) & ret) + ((-(secondQuarter)) & -ret);
    }

    constexpr Angle &operator+=(const Angle &a)
    {
        value += a.value;
        return *this;
    }

    constexpr Angle &operator-=(const Angle &a)
    {
        value -= a.value;
        return *this;
    }

    constexpr Angle operator+(const Angle &a) const { return Angle(*this) += a; }
    constexpr Angle operator-(const Angle &a) const { return Angle(*this) -= a; }
    constexpr bool operator>(const Angle &angle) const { return value > angle.value; }
    constexpr bool operator>=(const Angle &angle) const { return value >= angle.value; }
    constexpr bool operator<(const Angle &angle) const { return value < angle.value; }
    constexpr bool operator<=(const Angle &angle) const { return value <= angle.value; }
    constexpr bool operator==(const Angle &angle) const { return value == angle.value; }
    constexpr bool operator!=(const Angle &angle) const { return value != angle.value; }
};
