#pragma once

#include "..\Math\Fxp3D.hpp"

class Plane3D
{
public:
    Fxp3D normal;
    Fxp d;

    Plane3D() : normal(), d() {}

    Plane3D(const Fxp3D &normal, const Fxp3D &position)
    {
        this->normal = normal;
        d = normal.DotV2(position);
    }

    Plane3D(const Fxp3D &vertexA, const Fxp3D &vertexB, const Fxp3D &vertexC)
    {
        normal = Fxp3D::CalcNormal(vertexA, vertexB, vertexC);
        d = normal.DotV2(vertexB);
    }

    Fxp Distance(const Fxp3D &point) const
    {
        return d - normal.DotV2(point);
    }
};
