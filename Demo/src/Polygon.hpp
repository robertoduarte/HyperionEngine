#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"

#include "..\..\Math\Vec3.hpp"
#include "..\..\GL\GL.hpp"

using namespace Hyperion::GL;

enum class SortType : uint16_t
{
    Bfr = 0,
    Minimum = 1,
    Maximum = 2,
    Center = 3
};

enum class PlaneType : uint16_t
{
    Single = 0,
    Double = 1
};

enum class UseTexture : uint16_t
{
    No = 0,
    Yes = 1
};

struct Flags
{
    uint16_t : 11;
    SortType sortType : 3;
    PlaneType planeType : 1;
    UseTexture useTexture : 1;

    constexpr Flags(const SortType& sortType, const PlaneType& planeType, const UseTexture& useTexture) :
        sortType(sortType), planeType(planeType), useTexture(useTexture)
    {
    }
};

struct Indices
{
    uint16_t p0 = 0;
    uint16_t p1 = 0;
    uint16_t p2 = 0;
    uint16_t p3 = 0;

    constexpr Indices(const uint16_t& p0, const uint16_t& p1, const uint16_t& p2, const uint16_t& p3) :
        p0(p0), p1(p1), p2(p2), p3(p3)
    {
    }
};

struct Polygon
{
    Flags flags;
    Indices indices;

    constexpr Polygon(const Flags& flags, const Indices& indices) : flags(flags), indices(indices) {}
};

struct Mesh : public YaulType<Mesh, mesh_t>
{
    const Vec3* points;
    uint32_t pointCount;
    const Vec3* normals;
    const Polygon* polygons;
    const Attribute* attributes;
    uint32_t polygonCount;

    template <size_t PointCount, size_t PolygonCount>
    constexpr Mesh
    (
        const Vec3(&points)[PointCount],
        const Polygon(&polygons)[PolygonCount],
        const Attribute(&attributes)[PolygonCount]
    ) :
        points(points),
        pointCount(PointCount),
        normals(nullptr),
        polygons(polygons),
        attributes(attributes),
        polygonCount(PolygonCount)
    {
    }

    template <size_t PointCount, size_t PolygonCount>
    constexpr Mesh
    (
        const Vec3(&points)[PointCount],
        const Vec3(&normals)[PointCount],
        const Polygon(&polygons)[PolygonCount],
        const Attribute(&attributes)[PolygonCount]
    ) :
        points(points),
        pointCount(PointCount),
        normals(normals),
        polygons(polygons),
        attributes(attributes),
        polygonCount(PolygonCount)
    {
    }
};

#pragma GCC diagnostic pop
