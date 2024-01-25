#pragma once

#include "Plane3D.hpp"
#include "stddef.h"
#include "Trigonometry.hpp"

// xAxis = (Vec3*)&matrix[0][0];
// yAxis = (Vec3*)&matrix[1][0];
// zAxis = (Vec3*)&matrix[2][0];
// position = (Vec3*)&matrix[3][0];

/**
 * @brief Represents a view frustum in 3D space.
 */
class Frustum
{
private:
    // Named constants for plane indices
    static constexpr size_t PLANE_NEAR = 0;
    static constexpr size_t PLANE_FAR = 1;
    static constexpr size_t PLANE_TOP = 2;
    static constexpr size_t PLANE_BOTTOM = 3;
    static constexpr size_t PLANE_LEFT = 4;
    static constexpr size_t PLANE_RIGHT = 5;
    static constexpr size_t PLANE_COUNT = 6;

    Plane3D plane[PLANE_COUNT];

    Fxp nearDistance;   /**< Near clipping plane distance. */
    Fxp farDistance;    /**< Far clipping plane distance. */
    Fxp farWidth;       /**< Width of the far plane. */
    Fxp farHeight;      /**< Height of the far plane. */

public:
    /**
     * @brief Constructor to initialize the frustum.
     * @param verticalFov Vertical field of view.
     * @param ratio Aspect ratio.
     * @param nearDistance Near clipping plane distance.
     * @param farDistance Far clipping plane distance.
     */
    Frustum(const Fxp& verticalFov, const Fxp& ratio, const Fxp& nearDistance, const Fxp& farDistance)
        : farHeight(Trigonometry::Tan(verticalFov)),
        farWidth(farHeight* ratio),
        nearDistance(nearDistance),
        farDistance(farDistance)
    {
    }


    /**
     * @brief Update the frustum based on the view matrix.
     * @param position The position of the view.
     * @param xAxis The X-axis of the view matrix.
     * @param yAxis The Y-axis of the view matrix.
     * @param zAxis The Z-axis of the view matrix.
     */
    void Update(const Vec3& position, const Vec3& xAxis, const Vec3& yAxis, const Vec3& zAxis)
    {
        Vec3 farCentre(position + zAxis);
        Vec3 farHalfHeight(yAxis * farHeight);
        Vec3 farHalfWidth(yAxis * farWidth);

        Vec3 farTop(farCentre + farHalfHeight);
        Vec3 farTopLeft(farTop - farHalfWidth);
        Vec3 farTopRight(farTop + farHalfWidth);

        Vec3 farBottom(farCentre - farHalfHeight);
        Vec3 farBottomRight(farBottom + farHalfWidth);
        Vec3 farBottomLeft(farBottom - farHalfWidth);

        plane[PLANE_NEAR] = Plane3D(-zAxis, position + zAxis * nearDistance);
        plane[PLANE_FAR] = Plane3D(zAxis, position + zAxis * farDistance);
        plane[PLANE_TOP] = Plane3D(farTopRight, position, farTopLeft);
        plane[PLANE_BOTTOM] = Plane3D(farBottomLeft, position, farBottomRight);
        plane[PLANE_LEFT] = Plane3D(farTopLeft, position, farBottomLeft);
        plane[PLANE_RIGHT] = Plane3D(farBottomRight, position, farTopRight);
    }


    /**
     * @brief Update the frustum based on the view matrix components.
     *
     * This function updates the frustum using the provided position and z-axis.
     * It calculates the corresponding x-axis and y-axis based on a standard
     * right-handed coordinate system and then updates the frustum planes accordingly.
     *
     * @param position The position of the view.
     * @param zAxis The Z-axis of the view matrix.
     */
    void Update(const Vec3& position, const Vec3& zAxis)
    {
        // Extract yAxis and xAxis from zAxis (assuming a standard right-handed coordinate system)
        Vec3 yAxis = Vec3(0.0, 1.0, 0.0);  // Up vector
        Vec3 xAxis = yAxis.Cross(zAxis).Normalize();

        // Reuse the existing Update logic with extracted xAxis, yAxis, and zAxis
        Update(position, xAxis, yAxis, zAxis);
        // Add any additional logic specific to this overload, if needed
    }

    /**
     * @brief Check if a point is inside the frustum.
     * @param position The point to check.
     * @return True if the point is inside the frustum, false otherwise.
     */
    bool PointInFrustum(const Vec3& position) const
    {
        for (size_t i = 0; i < PLANE_COUNT; i++)
        {
            if (plane[i].Distance(position) < 0.0)
                return false;
        }
        return true;
    }

    /**
     * @brief Check if a sphere is inside the frustum.
     * @param position The center of the sphere.
     * @param size The size (diameter) of the sphere.
     * @return True if the sphere is inside the frustum, false otherwise.
     */
    bool SphereInFrustum(const Vec3& position, Fxp size) const
    {
        Fxp radius = size / 2.0;

        for (size_t i = 0; i < PLANE_COUNT; i++)
        {
            Fxp distance = plane[i].Distance(position);

            if (distance < -radius)
                return false;
        }

        return true;
    }

    /**
     * @brief Get a vertex of a box based on its position, size, normal, and direction.
     *
     * This function calculates a vertex of a box based on its position, size, normal,
     * and the direction specified by the template parameter.
     *
     * @tparam getPositiveVector Set to true for the positive direction, false for the negative direction.
     * @param position The position of the box.
     * @param size The size of the box.
     * @param normal The normal vector of the box.
     * @return The calculated vertex of the box.
     */
    template <bool getPositiveVector>
    Vec3 GetVertex(const Vec3& position, const Fxp& size, const Vec3& normal) const
    {
        Fxp halfExtent = size / 2.0;

        if constexpr (!getPositiveVector)
            halfExtent = -halfExtent;

        Vec3 result;
        result.x = (normal.x >= 0.0) ? position.x + halfExtent : position.x - halfExtent;
        result.y = (normal.y >= 0.0) ? position.y + halfExtent : position.y - halfExtent;
        result.z = (normal.z >= 0.0) ? position.z + halfExtent : position.z - halfExtent;

        return result;
    }

    /**
     * @brief Check if an axis-aligned box is inside the frustum.
     * @param position The position of the box.
     * @param size The size of the box.
     * @return True if the box is inside the frustum, false otherwise.
     */
    bool BoxInFrustum(const Vec3& position, const Fxp& size) const
    {
        Vec3 vertexN;
        for (size_t i = 0; i < PLANE_COUNT; i++)
        {
            vertexN = GetVertex<false>(position, size, plane[i].normal);
            if (plane[i].Distance(vertexN) < 0.0)
                return false;
        }
        return true;
    }
};
