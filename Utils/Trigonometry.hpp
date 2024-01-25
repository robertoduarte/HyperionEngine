#pragma once

#include "..\Math\Fxp.hpp"
#include "..\std\type_traits.h"

class Trigonometry
{
private:
    /**
     * @brief LookupCache struct for trigonometric values.
     * @tparam R The result type.
     * @tparam Mask A bitmask used for indexing.
     * @tparam InterpolationShift The shift value used for interpolation.
     */
    template <typename R, size_t Mask, size_t InterpolationShift>
    struct LookupCache
    {
        static constexpr size_t interpolationShift = InterpolationShift;

        R value; ///< The stored value.
        R interpolationMultiplicand; ///< Interpolation multiplicand.

        /**
         * @brief Extract a value from the lookup cache.
         * @param input The input value for interpolation.
         * @return The extracted value.
         */
        constexpr R ExtractValue(const auto& input)
        {
            size_t interpolationMultiplier = Mask & input;

            if constexpr (std::is_signed_v<R>)
                if (interpolationMultiplicand < 0)
                    return value - (R)((interpolationMultiplier * (size_t)(-interpolationMultiplicand)) >> InterpolationShift);

            return value + (R)((interpolationMultiplier * (size_t)interpolationMultiplicand) >> InterpolationShift);
        }
    };

    // Lookup table for sine values
    static constexpr LookupCache<int32_t, 0x3FF, 15> sinTable[] = {
        {Fxp(0.000000).value, 205556},  // Sine value for 0 degrees
        {Fxp(0.098017).value, 203577},  // Sine value for 5.625 degrees
        {Fxp(0.195090).value, 199637},  // Sine value for 11.25 degrees
        {Fxp(0.290285).value, 193774},  // Sine value for 16.875 degrees
        {Fxp(0.382683).value, 186045},  // Sine value for 22.5 degrees
        {Fxp(0.471397).value, 176524},  // Sine value for 28.125 degrees
        {Fxp(0.555570).value, 165303},  // Sine value for 33.75 degrees
        {Fxp(0.634393).value, 152491},  // Sine value for 39.375 degrees
        {Fxp(0.707107).value, 138210},  // Sine value for 45 degrees
        {Fxp(0.773010).value, 122597},  // Sine value for 50.625 degrees
        {Fxp(0.831470).value, 105804},  // Sine value for 56.25 degrees
        {Fxp(0.881921).value, 87992},   // Sine value for 61.875 degrees
        {Fxp(0.923880).value, 69333},   // Sine value for 67.5 degrees
        {Fxp(0.956940).value, 50006},   // Sine value for 73.125 degrees
        {Fxp(0.980785).value, 30197},   // Sine value for 78.75 degrees
        {Fxp(0.995185).value, 10098},   // Sine value for 84.375 degrees
        {Fxp(1.000000).value, -10098},  // Sine value for 90 degrees
        {Fxp(0.995185).value, -30197},  // Sine value for 95.625 degrees
        {Fxp(0.980785).value, -50006},  // Sine value for 101.25 degrees
        {Fxp(0.956940).value, -69333},  // Sine value for 106.875 degrees
        {Fxp(0.923880).value, -87992},  // Sine value for 112.5 degrees
        {Fxp(0.881921).value, -105804}, // Sine value for 118.125 degrees
        {Fxp(0.831470).value, -122597}, // Sine value for 123.75 degrees
        {Fxp(0.773010).value, -138210}, // Sine value for 129.375 degrees
        {Fxp(0.707107).value, -152491}, // Sine value for 135 degrees
        {Fxp(0.634393).value, -165303}, // Sine value for 140.625 degrees
        {Fxp(0.555570).value, -176524}, // Sine value for 146.25 degrees
        {Fxp(0.471397).value, -186045}, // Sine value for 151.875 degrees
        {Fxp(0.382683).value, -193774}, // Sine value for 157.5 degrees
        {Fxp(0.290285).value, -199637}, // Sine value for 163.125 degrees
        {Fxp(0.195090).value, -203577}, // Sine value for 168.75 degrees
        {Fxp(0.098017).value, -205556}, // Sine value for 174.375 degrees
        {Fxp(0.000000).value, -205556}, // Sine value for 180 degrees
        {Fxp(-0.098017).value, -203577}, // Sine value for -174.375 degrees
        {Fxp(-0.195090).value, -199637}, // Sine value for -168.75 degrees
        {Fxp(-0.290285).value, -193774}, // Sine value for -163.125 degrees
        {Fxp(-0.382683).value, -186045}, // Sine value for -157.5 degrees
        {Fxp(-0.471397).value, -176524}, // Sine value for -151.875 degrees
        {Fxp(-0.555570).value, -165303}, // Sine value for -146.25 degrees
        {Fxp(-0.634393).value, -152491}, // Sine value for -140.625 degrees
        {Fxp(-0.707107).value, -138210}, // Sine value for -135 degrees
        {Fxp(-0.773010).value, -122597}, // Sine value for -129.375 degrees
        {Fxp(-0.831470).value, -105804}, // Sine value for -123.75 degrees
        {Fxp(-0.881921).value, -87992},  // Sine value for -118.125 degrees
        {Fxp(-0.923880).value, -69333},  // Sine value for -112.5 degrees
        {Fxp(-0.956940).value, -50006},  // Sine value for -106.875 degrees
        {Fxp(-0.980785).value, -30197},  // Sine value for -101.25 degrees
        {Fxp(-0.995185).value, -10098},  // Sine value for -95.625 degrees
        {Fxp(-1.000000).value, 10098},   // Sine value for -90 degrees
        {Fxp(-0.995185).value, 30197},   // Sine value for -84.375 degrees
        {Fxp(-0.980785).value, 50006},   // Sine value for -78.75 degrees
        {Fxp(-0.956940).value, 69333},   // Sine value for -73.125 degrees
        {Fxp(-0.923880).value, 87992},   // Sine value for -67.5 degrees
        {Fxp(-0.881921).value, 105804},  // Sine value for -61.875 degrees
        {Fxp(-0.831470).value, 122597},  // Sine value for -56.25 degrees
        {Fxp(-0.773010).value, 138210},  // Sine value for -50.625 degrees
        {Fxp(-0.707107).value, 152491},  // Sine value for -45 degrees
        {Fxp(-0.634393).value, 165303},  // Sine value for -39.375 degrees
        {Fxp(-0.555570).value, 176524},  // Sine value for -33.75 degrees
        {Fxp(-0.471397).value, 186045},  // Sine value for -28.125 degrees
        {Fxp(-0.382683).value, 193774},  // Sine value for -22.5 degrees
        {Fxp(-0.290285).value, 199637},  // Sine value for -16.875 degrees
        {Fxp(-0.195090).value, 203577},  // Sine value for -11.25 degrees
        {Fxp(-0.098017).value, 205556}   // Sine value for -5.625 degrees
    };

    // Lookup tables for tangent values
    static constexpr LookupCache<int32_t, 0x3FF, 10> tanTable1[] = {
        {Fxp(0.00000).value, 6454},
        {Fxp(0.09849).value, 6581},
        {Fxp(0.19891).value, 6844},
        {Fxp(0.30335).value, 7265},
        {Fxp(0.41421).value, 7883},
        {Fxp(0.53451).value, 8760},
        {Fxp(0.66818).value, 9994},
        {Fxp(0.82068).value, 11751},
        {Fxp(1.00000).value, 14319},
        {Fxp(1.21850).value, 18225},
        {Fxp(1.49661).value, 24527},
        {Fxp(2.41421).value, 57825},
        {Fxp(3.29656).value, 113428},
        {Fxp(5.02734).value, 335926}
    };

    static constexpr LookupCache<int32_t, 0x0FF, 8> tanTable2[] = {
        {Fxp(10.15317).value, 223051},
        {Fxp(13.55667).value, 445566},
        {Fxp(20.35547).value, 1335624}
    };

    static constexpr LookupCache<int32_t, 0x03F, 6> tanTable3[] = {
        {Fxp(40.73548).value, 890193},
        {Fxp(54.31875).value, 1780251},
        {Fxp(81.48324).value, 5340487}
    };

    static constexpr LookupCache<int32_t, 0x00F, 4> tanTable4[] = {
        {Fxp(162.97262).value, 3560269},
        {Fxp(217.29801).value, 7120505},
        {Fxp(325.94830).value, 21361448}
    };

    static constexpr LookupCache<int32_t, 0x003, 2> tanTable5[] = {
        {Fxp(51.89814).value, 14240951},
        {Fxp(69.19781).value, 28481894},
        {Fxp(303.79704).value, 85445668},
        {Fxp(607.59446).value, 365979601},
        {0x7FFFFFFF, 0}
    };

    // Lookup table for atan2 values
    static constexpr LookupCache<uint16_t, 0x7FF, 17> aTan2Table[] = {
        {0, 20853},
        {326, 20813},
        {651, 20732},
        {975, 20612},
        {1297, 20454},
        {1617, 20260},
        {1933, 20032},
        {2246, 19773},
        {2555, 19484},
        {2860, 19170},
        {3159, 18832},
        {3453, 18474},
        {3742, 18098},
        {4025, 17708},
        {4302, 17306},
        {4572, 16896},
        {4836, 16479},
        {5094, 16058},
        {5344, 15635},
        {5589, 15212},
        {5826, 14790},
        {6058, 14372},
        {6282, 13959},
        {6500, 13552},
        {6712, 13151},
        {6917, 12759},
        {7117, 12374},
        {7310, 11999},
        {7498, 11633},
        {7679, 11277},
        {7856, 10931},
        {8026, 10595},
        {8192, 0}
    };

    static constexpr int32_t pi = Fxp(0.5).value;
    static constexpr int32_t halfPi = pi / 2;

public:
    static constexpr double RadPi = 3.14159265358979323846;

    static ANGLE RadiansToSgl(Fxp radians) { return (ANGLE)((radians * Fxp(180.0 / RadPi)) / Fxp(360.0)).Value(); }

    static int RadiansToDeg(Fxp radians) { return ((radians * Fxp(180.0 / RadPi)) >> 16).Value(); }

    static consteval Fxp RadiansToAngle(double radians) { return radians / (2.0 * RadPi); }

    static consteval Fxp DegreesToAngle(double degrees) { return degrees / 360.0; }

    static constexpr Fxp Sin(const Fxp& angle)
    {
        size_t index = (0xFFFF & angle.value) >> 10;
        auto tableValue = sinTable[index];
        return tableValue.ExtractValue(angle.value);
    }

    static constexpr Fxp Cos(const Fxp& angle)
    {
        size_t index = (0xFFFF & (angle.value + halfPi)) >> 10;
        auto tableValue = sinTable[index];
        return tableValue.ExtractValue(angle.value);
    }

    static constexpr Fxp Tan(const Fxp& angle)
    {
        int32_t tempAngle = 0xFFFF & angle.value;

        if (tempAngle >= pi)
            tempAngle += pi;

        // Determine if the angle is in the second quarter of the circle
        bool secondQuarter = tempAngle >= halfPi;

        // Adjust the angle for the second quarter if needed
        if (secondQuarter) tempAngle = pi - tempAngle;

        auto CalculateValue = [tempAngle, secondQuarter](auto lookupTable, auto upperRange)
        {
            size_t index = (tempAngle - upperRange) >> lookupTable[0].interpolationShift;
            auto tableValue = lookupTable[index];
            int32_t ret = tableValue.ExtractValue(tempAngle);
            return secondQuarter ? -ret : ret;
        };

        // Depending on the range of the angle, select the appropriate lookup table and perform interpolation

        // If angle is in the range [0x3FF0, 0xFFFF], use tanTable5
        if (tempAngle >= 0x3FF0) { return CalculateValue(tanTable5, 0x3FF0); }

        // If angle is in the range [0x3FC0, 0x3FF0], use tanTable4
        if (tempAngle >= 0x3FC0) { return CalculateValue(tanTable4, 0x3FC0); }

        // If angle is in the range [0x3F00, 0x3FC0], use tanTable3
        if (tempAngle >= 0x3F00) { return CalculateValue(tanTable3, 0x3F00); }

        // If angle is in the range [0x3C00, 0x3F00], use tanTable2
        if (tempAngle >= 0x3C00) { return CalculateValue(tanTable2, 0x3C00); }

        // If angle is in the range [0x0000, 0x3C00], use tanTable1
        return CalculateValue(tanTable1, 0);
    }

    /**
     * @brief Calculate the arctangent of the ratio `y` and `x`, returning the result in turns.
     *
     * It takes into account the signs of `x` and `y` to determine the correct quadrant for the result and
     * employs precomputed lookup tables and interpolation to perform the calculation. The final result is
     * returned in turns.
     *
     * @param y The y-coordinate.
     * @param x The x-coordinate.
     * @return The arctangent angle in turns.
     */
    static inline Fxp Atan2(const Fxp& x, const Fxp& y)
    {
        // Determine the initial result based on the signs of x and y
        int32_t result = x < 0.0 ? (y < 0.0 ? pi : -pi) : 0;

        int32_t divResult;

        // Check if the absolute value of x is less than the absolute value of y
        if (x.Abs() < y.Abs())
        {
            divResult = (x / y).value;
            // Adjust the result based on the quadrant
            result += divResult < 0.0 ? -halfPi : halfPi;
        }
        else
        {
            divResult = -(y / x).value;
        }

        // Adjust the divResult based on the sign
        if (divResult < 0)
        {
            divResult = -divResult;
            // Determine the index for table lookup and perform interpolation
            size_t index = divResult >> 11;
            auto tableValue = aTan2Table[index];
            return result + tableValue.ExtractValue(divResult);
        }

        // Determine the index for table lookup and perform interpolation
        size_t index = divResult >> 11;
        auto tableValue = aTan2Table[index];
        return result - tableValue.ExtractValue(divResult);
    }
};
