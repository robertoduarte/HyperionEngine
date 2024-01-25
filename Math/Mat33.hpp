#pragma once

#include "Vec3.hpp"
#include "Trigonometry.hpp"

struct Mat33
{
    Vec3 row0; /**< The first row of the 3x3 matrix. */
    Vec3 row1; /**< The second row of the 3x3 matrix. */
    Vec3 row2; /**< The third row of the 3x3 matrix. */

    /**
     * @brief Default constructor initializing a 3x3 matrix with zeros.
     */
    constexpr Mat33() : row0(), row1(), row2() {}

    /**
     * @brief Constructor initializing a 3x3 matrix with a given up vector and direction vector.
     * @param up The up vector.
     * @param direction The direction vector.
     */
    constexpr Mat33(const Vec3& up, const Vec3& direction) : row0(up.Cross(direction)), row1(up), row2(direction) {}

    /**
     * @brief Constructor initializing a 3x3 matrix with specified rows.
     * @param row0In The first row of the matrix.
     * @param row1In The second row of the matrix.
     * @param row2In The third row of the matrix.
     */
    constexpr Mat33(const Vec3& row0In, const Vec3& row1In, const Vec3& row2In) : row0(row0In), row1(row1In), row2(row2In) {}

    /**
     * @brief Multiply this matrix by another matrix.
     * @param other The matrix to multiply with.
     * @return The result of the matrix multiplication.
     */
    Mat33 operator*(const Mat33& other) const
    {
        Mat33 transposed = other;
        transposed.Transpose();

        return Mat33(
            Vec3(row0.Dot(transposed.row0), row0.Dot(transposed.row1), row0.Dot(transposed.row2)),
            Vec3(row1.Dot(transposed.row0), row1.Dot(transposed.row1), row1.Dot(transposed.row2)),
            Vec3(row2.Dot(transposed.row0), row2.Dot(transposed.row1), row2.Dot(transposed.row2))
        );
    }

    /**
     * @brief Multiply this matrix by a 3D vector.
     * @param v The vector to multiply with.
     * @return The result of the matrix-vector multiplication.
     */
    Vec3 operator*(const Vec3& v) const { return Vec3(row0.Dot(v), row1.Dot(v), row2.Dot(v)); }

    /**
     * @brief Transposes the 3x3 matrix.
     *
     * This function swaps the elements of the matrix across its main diagonal.
     * It effectively transposes the matrix, converting rows into columns and columns into rows.
     * @return A reference to the modified matrix, allowing for method chaining.
     */
    Mat33& Transpose()
    {
        // Swap row0.y and row1.x
        const Fxp m01 = row0.y;
        row0.y = row1.x;
        row1.x = m01;

        // Swap row0.z and row2.x
        const Fxp m02 = row0.z;
        row0.z = row2.x;
        row2.x = m02;

        // Swap row1.z and row2.y
        const Fxp m12 = row1.z;
        row1.z = row2.y;
        row2.y = m12;

        return *this;
    }

    /**
     * @brief Rotate the matrix around the X axis by the specified angle.
     * @param angleX The rotation angle.
     * @return A reference to the modified matrix, allowing for method chaining.
     */
    Mat33& RotateX(const Fxp& angleX)
    {
        // Compute sin and cos values for the angleX
        Fxp sinValue = Trigonometry::Sin(angleX);
        Fxp cosValue = Trigonometry::Cos(angleX);

        // Update matrix elements to perform rotation around the X-axis
        const Fxp m01 = row0.y;
        const Fxp m02 = row0.z;
        const Fxp m11 = row1.y;
        const Fxp m12 = row1.z;
        const Fxp m21 = row2.y;
        const Fxp m22 = row2.z;

        row0.y = (m01 * cosValue) + (m02 * sinValue);
        row0.z = -(m01 * sinValue) + (m02 * cosValue);

        row1.y = (m11 * cosValue) + (m12 * sinValue);
        row1.z = -(m11 * sinValue) + (m12 * cosValue);

        row2.y = (m21 * cosValue) + (m22 * sinValue);
        row2.z = -(m21 * sinValue) + (m22 * cosValue);

        return *this;
    }

    /**
     * @brief Rotate the matrix around the Y axis by the specified angle.
     * @param angleY The rotation angle.
     * @return A reference to the modified matrix, allowing for method chaining.
     */
    Mat33& RotateY(const Fxp& angleY)
    {
        // Compute sin and cos values for the angleY
        Fxp sinValue = Trigonometry::Sin(angleY);
        Fxp cosValue = Trigonometry::Cos(angleY);

        // Update matrix elements to perform rotation around the Y-axis
        const Fxp m00 = row0.x;
        const Fxp m02 = row0.z;
        const Fxp m10 = row1.x;
        const Fxp m12 = row1.z;
        const Fxp m20 = row2.x;
        const Fxp m22 = row2.z;

        row0.x = (m00 * cosValue) - (m02 * sinValue);
        row0.z = (m00 * sinValue) + (m02 * cosValue);

        row1.x = (m10 * cosValue) - (m12 * sinValue);
        row1.z = (m10 * sinValue) + (m12 * cosValue);

        row2.x = (m20 * cosValue) - (m22 * sinValue);
        row2.z = (m20 * sinValue) + (m22 * cosValue);

        return *this;
    }

    /**
     * @brief Rotate the matrix around the Z axis by the specified angle.
     * @param angleZ The rotation angle.
     * @return A reference to the modified matrix, allowing for method chaining.
     */
    Mat33& RotateZ(const Fxp& angleZ)
    {
        // Compute sin and cos values for the angleZ
        Fxp sinValue = Trigonometry::Sin(angleZ);
        Fxp cosValue = Trigonometry::Cos(angleZ);

        // Update matrix elements to perform rotation around the Z-axis
        const Fxp m00 = row0.x;
        const Fxp m01 = row0.y;
        const Fxp m10 = row1.x;
        const Fxp m11 = row1.y;
        const Fxp m20 = row2.x;
        const Fxp m21 = row2.y;

        row0.x = (m00 * cosValue) + (m01 * sinValue);
        row0.y = -(m00 * sinValue) + (m01 * cosValue);

        row1.x = (m10 * cosValue) + (m11 * sinValue);
        row1.y = -(m10 * sinValue) + (m11 * cosValue);

        row2.x = (m20 * cosValue) + (m21 * sinValue);
        row2.y = -(m20 * sinValue) + (m21 * cosValue);

        return *this;
    }

    /**
     * @brief Static function to create an identity 3x3 matrix.
     * @return The identity matrix.
     */
    static consteval Mat33 Identity()
    {
        return Mat33(
            Vec3(1.0, 0.0, 0.0),
            Vec3(0.0, 1.0, 0.0),
            Vec3(0.0, 0.0, 1.0)
        );
    }

    void PrintDebug()
    {
        dbgio_printf("m00:%f m01:%f m02:%f\n", row0.x.Value(), row0.y.Value(), row0.z.Value());
        dbgio_printf("m10:%f m11:%f m12:%f\n", row1.x.Value(), row1.y.Value(), row1.z.Value());
        dbgio_printf("m20:%f m21:%f m22:%f\n", row2.x.Value(), row2.y.Value(), row2.z.Value());
    }
};
