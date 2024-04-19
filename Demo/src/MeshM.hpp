#pragma once
#include "..\..\Math\Vec3.hpp"
#include "Polygon.hpp"

using namespace Hyperion::GL;

static inline constexpr Vec3 points_m[] = {
        Vec3(0.0,  0.0,  0.0), Vec3(5.0, -3.0,  2.0), Vec3(3.0, -3.0,  2.0), Vec3(3.0,  3.0,  2.0), Vec3(5.0,  3.0,  2.0),
        Vec3(5.0, -3.0, -2.0), Vec3(-3.0, -3.0, -2.0), Vec3(-3.0, -3.0,  2.0), Vec3(-3.0, -1.0,  2.0), Vec3(3.0, -1.0,  2.0),
        Vec3(-5.0, -1.0,  2.0), Vec3(-5.0, -1.0, -2.0), Vec3(-5.0,  3.0, -2.0), Vec3(-5.0,  3.0,  2.0), Vec3(-3.0,  3.0,  2.0),
        Vec3(-3.0,  3.0, -2.0), Vec3(-1.0, -1.0,  2.0), Vec3(-1.0, -1.0, -2.0), Vec3(-1.0,  3.0, -2.0), Vec3(-1.0,  3.0,  2.0),
        Vec3(1.0,  3.0,  2.0), Vec3(1.0,  3.0, -2.0), Vec3(1.0, -1.0,  2.0), Vec3(3.0, -1.0, -2.0), Vec3(3.0,  3.0, -2.0),
        Vec3(5.0,  3.0, -2.0), Vec3(3.0, -3.0, -2.0), Vec3(-3.0, -1.0, -2.0), Vec3(1.0, -1.0, -2.0)
};

static inline constexpr Attribute commonAttribute_m = Attribute(
        Control(Control::LinkType::JumpAssign, Control::CommandType::Polygon),
        DrawMode(DrawMode::EndCodeDisable::True, DrawMode::TransPixelDisable::True),
        Color::Rgb1555(25, 3, 3, 1));

static inline constexpr Attribute attributes_m[] = {
        commonAttribute_m, commonAttribute_m, commonAttribute_m, commonAttribute_m, commonAttribute_m,
        commonAttribute_m, commonAttribute_m, commonAttribute_m, commonAttribute_m, commonAttribute_m,
        commonAttribute_m, commonAttribute_m, commonAttribute_m, commonAttribute_m, commonAttribute_m,
        commonAttribute_m, commonAttribute_m, commonAttribute_m, commonAttribute_m, commonAttribute_m,
        commonAttribute_m, commonAttribute_m, commonAttribute_m
};

static inline constexpr Flags commonFlags_m = Flags(SortType::Center, PlaneType::Single, UseTexture::No);

static inline constexpr Polygon polygons_m[] = {
        Polygon(commonFlags_m, Indices(8, 10,  7,  7)),
        Polygon(commonFlags_m, Indices(11, 27,  6,  6)),
        Polygon(commonFlags_m, Indices(5,  1,  7,  6)),
        Polygon(commonFlags_m, Indices(7,  2,  9,  8)),
        Polygon(commonFlags_m, Indices(6,  7, 10, 11)),
        Polygon(commonFlags_m, Indices(11, 10, 13, 12)),
        Polygon(commonFlags_m, Indices(13, 14, 15, 12)),
        Polygon(commonFlags_m, Indices(10,  8, 14, 13)),
        Polygon(commonFlags_m, Indices(17, 16, 19, 18)),
        Polygon(commonFlags_m, Indices(19, 20, 21, 18)),
        Polygon(commonFlags_m, Indices(16, 22, 20, 19)),
        Polygon(commonFlags_m, Indices(23,  9,  3, 24)),
        Polygon(commonFlags_m, Indices(3,  4, 25, 24)),
        Polygon(commonFlags_m, Indices(1,  5, 25,  4)),
        Polygon(commonFlags_m, Indices(25,  5, 26, 24)),
        Polygon(commonFlags_m, Indices(23, 26,  6, 27)),
        Polygon(commonFlags_m, Indices(8, 27, 15, 14)),
        Polygon(commonFlags_m, Indices(15, 27, 11, 12)),
        Polygon(commonFlags_m, Indices(16, 17, 27,  8)),
        Polygon(commonFlags_m, Indices(22, 28, 21, 20)),
        Polygon(commonFlags_m, Indices(21, 28, 17, 18)),
        Polygon(commonFlags_m, Indices(9, 23, 28, 22)),
        Polygon(commonFlags_m, Indices(2,  1,  4,  3))
};

static inline constexpr Mesh mesh_m = Mesh(points_m, polygons_m, attributes_m);
