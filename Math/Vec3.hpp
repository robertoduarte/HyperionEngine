#pragma once

#include "Fxp.hpp"

/**
 * @brief A struct for three-dimensional fixed-point vector arithmetic operations.
 */
struct Vec3
{
    Fxp x; /**< The X-coordinate. */
    Fxp y; /**< The Y-coordinate. */
    Fxp z; /**< The Z-coordinate. */

    // Constructors
    /**
     * @brief Default constructor, initializes all coordinates to 0.
     */
    constexpr Vec3() : x(), y(), z() {}

    /**
     * @brief Copy constructor.
     * @param vec The Vec3 object to copy.
     */
    constexpr Vec3(const Vec3& vec) : x(vec.x), y(vec.y), z(vec.z) {}

    /**
     * @brief Constructor to initialize coordinates with specific values.
     * @param valueX The X-coordinate.
     * @param valueY The Y-coordinate.
     * @param valueZ The Z-coordinate.
     */
    constexpr Vec3(const Fxp& valueX, const Fxp& valueY, const Fxp& valueZ) : x(valueX), y(valueY), z(valueZ) {}

    // Assignment operator
    /**
     * @brief Assignment operator.
     * @param vec The Vec3 object to assign.
     * @return Reference to the modified Vec3 object.
     */
    constexpr Vec3& operator=(const Vec3& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }

    // Other member functions
    /**
     * @brief Calculate the absolute values of each coordinate.
     * @return A new Vec3 object with absolute values.
     */
    constexpr Vec3 Abs() const
    {
        return Vec3(x.Abs(), y.Abs(), z.Abs());
    }

    /**
     * @brief Sort coordinates in ascending order.
     * @tparam Ascending If true, sort in ascending order; otherwise, in descending order.
     * @return A new Vec3 object with sorted coordinates.
     */
    template <bool Ascending = true>
    constexpr Vec3 Sort()
    {
        Vec3 result(*this);
        Fxp temp;

        if (Ascending ? result.x > result.y : result.x < result.y)
        {
            temp = result.x;
            result.x = result.y;
            result.y = temp;
        }

        if (Ascending ? result.x > result.z : result.x < result.z)
        {
            temp = result.x;
            result.x = result.z;
            result.z = temp;
        }

        if (Ascending ? result.y > result.z : result.y < result.z)
        {
            temp = result.y;
            result.y = result.z;
            result.z = temp;
        }

        return result;
    }

    /**
     * @brief Calculate the dot product of this object and another Vec3 object.
     * @param vec The Vec3 object to calculate the dot product with.
     * @return The dot product as an Fxp value.
     */
    constexpr Fxp Dot(const Vec3& vec) const
    {
        if consteval
        {
            return x * vec.x + y * vec.y + z * vec.z;
        }
        else
        {
            int32_t aux0;
            int32_t aux1;
            auto a = reinterpret_cast<const int32_t*>(this);
            auto b = reinterpret_cast<const int32_t*>(&vec);

            __asm__ volatile("\tclrmac\n"
                "\tmac.l @%[a]+, @%[b]+\n"
                "\tmac.l @%[a]+, @%[b]+\n"
                "\tmac.l @%[a]+, @%[b]+\n"
                "\tsts mach, %[aux0]\n"
                "\tsts macl, %[aux1]\n"
                "\txtrct %[aux0], %[aux1]\n"
                : [a] "+r"(a),
                [b] "+r"(b),
                [aux0] "=&r"(aux0),
                [aux1] "=&r"(aux1)
                : "m"(*a),
                "m"(*b)
                : "mach", "macl", "memory");
            return aux1;
        }
    }

    /**
     * @brief Calculate the cross product of this object and another Vec3 object.
     * @param vec The Vec3 object to calculate the cross product with.
     * @return The cross product as an Vec3 object.
     */
    constexpr Vec3 Cross(const Vec3& vec) const
    {
        return Vec3(z * vec.y - y * vec.z,
            x * vec.z - z * vec.x,
            y * vec.x - x * vec.y);
    }

    // Scalar multiplication and division
    /**
     * @brief Multiply each coordinate by a scalar value.
     * @param fxp The scalar value to multiply by.
     * @return The resulting Vec3 object.
     */
    constexpr Vec3 operator*(const Fxp& fxp) const
    {
        return Vec3(x * fxp, y * fxp, z * fxp);
    }

    /**
     * @brief Divide each coordinate by a scalar value.
     * @param fxp The scalar value to divide by.
     * @return The resulting Vec3 object.
     */
    constexpr Vec3 operator/(const Fxp& fxp) const
    {
        return Vec3(x / fxp, y / fxp, z / fxp);
    }

    // Vector length calculation
    /**
     * @brief Calculate the length of the vector represented by this Vec3 object.
     * @return The length as an Fxp value.
     */
    constexpr Fxp Length() const
    {
        return Dot(*this).Sqrt();
    }

    /**
     * @brief Calculate the length of the vector represented by this Vec3 object using a fast approximation.
     * @return The length as an Fxp value.
     */
    constexpr Fxp FastLength() const
    {
        return Dot(*this).FastSqrt();
    }

    /**
     * @brief Calculate the length of the vector represented by this Vec3 object using a fast approximation.
     * @return The length as an Fxp value.
     */
    constexpr Fxp TurboLength() const
    {
        constexpr Vec3 alphaBetaGama =
            Vec3
            (
                0.9398086351723256, // Alpha
                0.38928148272372454, // Beta
                0.2987061876143797   // Gamma
            );

        return Abs().Sort<false>().Dot(alphaBetaGama);
    }

    // Vector normalization
    /**
     * @brief Normalize the vector represented by this Vec3 object.
     * @return A normalized Vec3 object.
     */
    constexpr Vec3 Normalize() const
    {
        Fxp length = Length();
        if (length != 0.0F)
            return Vec3(length / x, length / y, length / z);
        else
            return Vec3();
    }

    // Vector normalization
    /**
     * @brief Normalize the vector represented by this Vec3 object.
     * @return A normalized Vec3 object.
     */
    constexpr Vec3 FastNormalize() const
    {
        Fxp length = FastLength();
        if (length != 0.0F)
            return Vec3(length / x, length / y, length / z);
        else
            return Vec3();
    }

    // Vector normalization
      /**
       * @brief Normalize the vector represented by this Vec3 object.
       * @return A normalized Vec3 object.
       */
    constexpr Vec3 TurboNormalize() const
    {
        Fxp length = TurboLength();
        if (length != 0.0F)
            return Vec3(length / x, length / y, length / z);
        else
            return Vec3();
    }

    /**
     * @brief Calculate the normal vector of a triangle defined by three Vec3 vertices.
     * @param vertexA The first vertex.
     * @param vertexB The second vertex.
     * @param vertexC The third vertex.
     * @return The normal vector as an Vec3 object.
     */
    constexpr static Vec3 CalcNormal(const Vec3& vertexA, const Vec3& vertexB, const Vec3& vertexC)
    {
        return (vertexA - vertexB).Cross(vertexC - vertexB);
    }

    // Comparison operators
    /**
     * @brief Check if two Vec3 objects are not equal.
     * @param vec The Vec3 object to compare.
     * @return True if not equal, false otherwise.
     */
    constexpr bool operator!=(const Vec3& vec) const
    {
        return x != vec.x && y != vec.y && z != vec.z;
    }

    // Bitwise shift operators
    /**
     * @brief Bitwise right shift operator.
     * @param shiftAmount The number of positions to shift.
     * @return The resulting Vec3 object.
     */
    constexpr Vec3 operator>>(const size_t& shiftAmount)
    {
        return Vec3(x >> shiftAmount, y >> shiftAmount, z >> shiftAmount);
    }

    /**
     * @brief Bitwise right shift assignment operator.
     * @param shiftAmount The number of positions to shift.
     * @return Reference to the modified Vec3 object.
     */
    constexpr Vec3& operator>>=(const size_t& shiftAmount)
    {
        x >>= shiftAmount;
        y >>= shiftAmount;
        z >>= shiftAmount;
        return *this;
    }

    /**
     * @brief Bitwise left shift operator.
     * @param shiftAmount The number of positions to shift.
     * @return The resulting Vec3 object.
     */
    constexpr Vec3 operator<<(const size_t& shiftAmount)
    {
        return Vec3(x << shiftAmount, y << shiftAmount, z << shiftAmount);
    }

    /**
     * @brief Bitwise left shift assignment operator.
     * @param shiftAmount The number of positions to shift.
     * @return Reference to the modified Vec3 object.
     */
    constexpr Vec3& operator<<=(const size_t& shiftAmount)
    {
        x <<= shiftAmount;
        y <<= shiftAmount;
        z <<= shiftAmount;
        return *this;
    }

    // Unary operators
    /**
     * @brief Unary negation operator.
     * @return A new Vec3 object with negated coordinates.
     */
    constexpr Vec3 operator-() const
    {
        return Vec3(-x, -y, -z);
    }

    // Binary operators
    /**
     * @brief Binary addition operator.
     * @param vec The Vec3 object to add.
     * @return The sum as an Vec3 object.
     */
    constexpr Vec3 operator+(const Vec3& vec) const
    {
        return Vec3(x + vec.x, y + vec.y, z + vec.z);
    }

    /**
     * @brief Binary subtraction operator.
     * @param vec The Vec3 object to subtract.
     * @return The difference as an Vec3 object.
     */
    constexpr Vec3 operator-(const Vec3& vec) const
    {
        return Vec3(x - vec.x, y - vec.y, z - vec.z);
    }

    /**
     * @brief Compound addition assignment operator.
     * @param vec The Vec3 object to add.
     * @return Reference to the modified Vec3 object.
     */
    constexpr Vec3 operator+=(const Vec3& vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;
        return *this;
    }

    /**
     * @brief Compound subtraction assignment operator.
     * @param vec The Vec3 object to subtract.
     * @return Reference to the modified Vec3 object.
     */
    constexpr Vec3 operator-=(const Vec3& vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
        return *this;
    }
};
