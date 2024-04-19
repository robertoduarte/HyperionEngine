#pragma once
#include "..\..\Math\Vec3.hpp"
#include "Polygon.hpp"

using namespace Hyperion::GL;

static inline constexpr Vec3 points_c[] = {
        Vec3(0.0,  0.0,  0.0), Vec3(3.0, -3.0,  2.0), Vec3(-1.0, -3.0,  2.0),
        Vec3(-1.0, -1.0,  2.0), Vec3(3.0, -1.0,  2.0), Vec3(-3.0, -1.0,  2.0),
        Vec3(1.0, -1.0,  2.0), Vec3(1.0,  3.0,  2.0), Vec3(3.0,  3.0,  2.0),
        Vec3(1.0,  1.0,  2.0), Vec3(-3.0,  1.0,  2.0), Vec3(-3.0,  3.0,  2.0),
        Vec3(3.0, -3.0, -2.0), Vec3(-1.0, -3.0, -2.0), Vec3(-1.0, -1.0, -2.0),
        Vec3(3.0, -1.0, -2.0), Vec3(-3.0, -1.0, -2.0), Vec3(1.0, -1.0, -2.0),
        Vec3(1.0,  3.0, -2.0), Vec3(3.0,  3.0, -2.0), Vec3(1.0,  1.0, -2.0),
        Vec3(-3.0,  1.0, -2.0), Vec3(-3.0,  3.0, -2.0), Vec3(1.0, -3.0, -2.0),
        Vec3(1.0, -3.0,  2.0), Vec3(3.0,  1.0,  2.0), Vec3(3.0,  1.0, -2.0)
};

static inline constexpr Attribute commonAttribute_c = Attribute(
        Control(Control::LinkType::JumpAssign, Control::CommandType::Polygon),
        DrawMode(DrawMode::EndCodeDisable::True, DrawMode::TransPixelDisable::True),
        Color::Rgb1555(3, 3, 25, 1));


static inline constexpr Attribute attributes_c[] = {
        commonAttribute_c, commonAttribute_c, commonAttribute_c, commonAttribute_c, commonAttribute_c,
        commonAttribute_c, commonAttribute_c, commonAttribute_c, commonAttribute_c, commonAttribute_c,
        commonAttribute_c, commonAttribute_c, commonAttribute_c, commonAttribute_c, commonAttribute_c,
        commonAttribute_c, commonAttribute_c, commonAttribute_c, commonAttribute_c, commonAttribute_c,
        commonAttribute_c, commonAttribute_c, commonAttribute_c, commonAttribute_c, commonAttribute_c
};

static inline constexpr Flags commonFlags_c = Flags(SortType::Center, PlaneType::Single, UseTexture::No);

static inline constexpr Polygon polygons_c[] = {
        Polygon(commonFlags_c, Indices(8, 25, 26, 19)) ,
        Polygon(commonFlags_c, Indices(4,  1, 12, 15)) ,
        Polygon(commonFlags_c, Indices(25,  4, 15, 26)) ,
        Polygon(commonFlags_c, Indices(24,  6,  3,  2)) ,
        Polygon(commonFlags_c, Indices(4,  6, 24,  1)) ,
        Polygon(commonFlags_c, Indices(2, 13, 23, 24)) ,
        Polygon(commonFlags_c, Indices(1, 24, 23, 12)) ,
        Polygon(commonFlags_c, Indices(17, 14,  3,  6)) ,
        Polygon(commonFlags_c, Indices(16,  5,  3, 14)) ,
        Polygon(commonFlags_c, Indices(3,  5,  2,  2)) ,
        Polygon(commonFlags_c, Indices(13,  2,  5, 16)) ,
        Polygon(commonFlags_c, Indices(14, 14, 13, 16)) ,
        Polygon(commonFlags_c, Indices(23, 13, 14, 17)) ,
        Polygon(commonFlags_c, Indices(12, 23, 17, 15)) ,
        Polygon(commonFlags_c, Indices(17,  6,  9, 20)) ,
        Polygon(commonFlags_c, Indices(10, 21, 20,  9)) ,
        Polygon(commonFlags_c, Indices(11, 22, 21, 10)) ,
        Polygon(commonFlags_c, Indices(7, 11, 10,  9)) ,
        Polygon(commonFlags_c, Indices(4, 25,  9,  6)) ,
        Polygon(commonFlags_c, Indices(25,  8,  7,  9)) ,
        Polygon(commonFlags_c, Indices(8, 19, 18,  7)) ,
        Polygon(commonFlags_c, Indices(11,  7, 18, 22)) ,
        Polygon(commonFlags_c, Indices(26, 20, 18, 19)) ,
        Polygon(commonFlags_c, Indices(18, 20, 21, 22)) ,
        Polygon(commonFlags_c, Indices(15, 17, 20, 26))
};

static inline constexpr Mesh mesh_c = Mesh(points_c, polygons_c, attributes_c);
