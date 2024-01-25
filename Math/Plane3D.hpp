#pragma once

#include "..\Math\Vec3.hpp"

/**
 * @brief Represents a 3D plane.
 */
class Plane3D
{
public:
    Vec3 normal; /**< Normal vector of the plane. */
    Fxp d;     /**< Offset or signed distance of the plane. */

    /**
     * @brief Default constructor.
     */
    Plane3D() : normal(), d() {}

    /**
     * @brief Constructor with a given normal vector and offset.
     */
    Plane3D(const Vec3& normal, Fxp d) : normal(normal), d(d) {}

    /**
     * @brief Constructor with a given normal vector and a point on the plane.
     */
    Plane3D(const Vec3& normal, const Vec3& position) : normal(normal), d(normal.Dot(position)) {}

    /**
     * @brief Constructor with three vertices defining the plane.
     */
    Plane3D(const Vec3& vertexA, const Vec3& vertexB, const Vec3& vertexC)
    {
        normal = Vec3::CalcNormal(vertexA, vertexB, vertexC);
        d = normal.Dot(vertexB);
    }

    /**
     * @brief Calculate the signed distance from a point to the plane.
     * @param point The point in 3D space.
     * @return The signed distance from the point to the plane.
     */
    Fxp Distance(const Vec3& point) const
    {
        return d - normal.Dot(point);
    }
};
