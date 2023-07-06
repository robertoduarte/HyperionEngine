#pragma once

#include "Plane3D.hpp"
#include "stddef.h"

class Frustum
{
private:
    Fxp nearDistance;
    Fxp farDistance;
    Fxp farWidth;
    Fxp farHeight;

    enum Plane
    {
        Near,
        Far,
        Top,
        Bottom,
        Left,
        Right,
        Count
    };
    Plane3D plane[Plane::Count];

public:
    Frustum(Fxp verticalFov, Fxp ratio, Fxp nearDistance, Fxp farDistance)
    {
        farHeight = verticalFov.Tan();
        farWidth = farHeight * ratio;
        this->nearDistance = nearDistance;
        this->farDistance = farDistance;
    }

    // xAxis = (Fxp3D*)&matrix[0][0];
    // yAxis = (Fxp3D*)&matrix[1][0];
    // zAxis = (Fxp3D*)&matrix[2][0];
    // position = (Fxp3D*)&matrix[3][0];

    void Update(const Fxp3D &position, const Fxp3D &xAxis, const Fxp3D &yAxis, const Fxp3D &zAxis)
    {
        Fxp3D farCentre(position + zAxis);
        Fxp3D farHalfHeight(yAxis * farHeight);
        Fxp3D farHalfWidth(yAxis * farWidth);

        Fxp3D farTop(farCentre + farHalfHeight);
        Fxp3D farTopLeft(farTop - farHalfWidth);
        Fxp3D farTopRight(farTop + farHalfWidth);

        Fxp3D farBottom(farCentre - farHalfHeight);
        Fxp3D farBottomRight(farBottom + farHalfWidth);
        Fxp3D farBottomLeft(farBottom - farHalfWidth);
        
        plane[Plane::Near] = Plane3D(-zAxis, position + zAxis * nearDistance);
        plane[Plane::Far] = Plane3D(zAxis, position + zAxis * farDistance);
        plane[Plane::Top] = Plane3D(farTopRight, position, farTopLeft);
        plane[Plane::Bottom] = Plane3D(farBottomLeft, position, farBottomRight);
        plane[Plane::Left] = Plane3D(farTopLeft, position, farBottomLeft);
        plane[Plane::Right] = Plane3D(farBottomRight, position, farTopRight);
    }

    bool PointInFrustum(const Fxp3D &position) const
    {
        for (size_t i = 0; i < Plane::Count; i++)
        {
            if (plane[i].Distance(position) < 0.0F)
                return false;
        }
        return true;
    }

    bool SphereInFrustum(const Fxp3D &position, Fxp size) const
    {
        Fxp radius = size / 2.0F;
        for (size_t i = 0; i < Plane::Count; i++)
        {
            if (plane[i].Distance(position) < -radius)
                return false;
        }
        return true;
    }

    Fxp3D GetVertex(const Fxp3D &position, Fxp size, const Fxp3D &normal, bool getPositiveVector) const
    {
        Fxp halfsize = size / 2.0F;
        Fxp halfExtent = (!getPositiveVector) ? halfsize : -halfsize;

        Fxp3D result;
        result.x = (normal.x >= 0.0F) ? position.x + halfExtent : position.x - halfExtent;
        result.y = (normal.y >= 0.0F) ? position.y + halfExtent : position.y - halfExtent;
        result.z = (normal.z >= 0.0F) ? position.z + halfExtent : position.z - halfExtent;

        return result;
    }

    bool BoxInFrustum(const Fxp3D &position, Fxp size) const
    {
        Fxp3D vertexN;
        for (size_t i = 0; i < Plane::Count; i++)
        {
            vertexN = GetVertex(position, size, plane[i].normal, false);
            if (plane[i].Distance(vertexN) < 0.0F)
                return false;
        }
        return true;
    }
};
