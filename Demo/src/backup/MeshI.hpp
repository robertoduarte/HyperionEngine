#pragma once
#include "..\..\Math\Vec3.hpp"
#include "Polygon.hpp"

using namespace Hyperion::GL;

static inline constexpr Vec3 points_i[] = {
        Vec3(0.0,  0.0,  0.0),
        Vec3(1.0, -3.0,  2.0),
        Vec3(-1.0, -1.0,  2.0),
        Vec3(-1.0,  3.0,  2.0),
        Vec3(1.0,  3.0,  2.0),
        Vec3(1.0, -3.0, -2.0),
        Vec3(-1.0, -1.0, -2.0),
        Vec3(-1.0,  3.0, -2.0),
        Vec3(1.0,  3.0, -2.0)
};

static inline const attribute_t attributes_i[6] = {
        {.draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        {.draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        {.draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        {.draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        {.draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        {.draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 }
};

static inline const polygon_t polygons_i[6] = {
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(4, 8, 7, 3) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(2, 6, 5, 1) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(3, 7, 6, 2) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(5, 6, 7, 8) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(3, 2, 1, 4) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(5, 8, 4, 1) }
};

static inline const  mesh_t mesh_i = {
        .points = points_i,
        .points_count = 9,
        .polygons = polygons_i,
        .attributes = attributes_i,
        .polygons_count = 6
};
