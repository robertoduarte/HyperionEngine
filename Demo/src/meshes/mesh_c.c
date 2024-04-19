#include "mesh.h"

#define COLOR1 RGB1555(1, 3, 3, 25)

static const fix16_vec3_t _points_c[27] = {
        FIX16_VEC3_INITIALIZER( 0,  0,  0),
        FIX16_VEC3_INITIALIZER( 3, -3,  2),
        FIX16_VEC3_INITIALIZER(-1, -3,  2),
        FIX16_VEC3_INITIALIZER(-1, -1,  2),
        FIX16_VEC3_INITIALIZER( 3, -1,  2),
        FIX16_VEC3_INITIALIZER(-3, -1,  2),
        FIX16_VEC3_INITIALIZER( 1, -1,  2),
        FIX16_VEC3_INITIALIZER( 1,  3,  2),
        FIX16_VEC3_INITIALIZER( 3,  3,  2),
        FIX16_VEC3_INITIALIZER( 1,  1,  2),
        FIX16_VEC3_INITIALIZER(-3,  1,  2),
        FIX16_VEC3_INITIALIZER(-3,  3,  2),
        FIX16_VEC3_INITIALIZER( 3, -3, -2),
        //
        FIX16_VEC3_INITIALIZER(-1, -3, -2),
        FIX16_VEC3_INITIALIZER(-1, -1, -2),
        FIX16_VEC3_INITIALIZER( 3, -1, -2),
        FIX16_VEC3_INITIALIZER(-3, -1, -2),
        FIX16_VEC3_INITIALIZER( 1, -1, -2),
        FIX16_VEC3_INITIALIZER( 1,  3, -2),
        FIX16_VEC3_INITIALIZER( 3,  3, -2),
        FIX16_VEC3_INITIALIZER( 1,  1, -2),
        FIX16_VEC3_INITIALIZER(-3,  1, -2),
        FIX16_VEC3_INITIALIZER(-3,  3, -2),
        FIX16_VEC3_INITIALIZER( 1, -3, -2),
        FIX16_VEC3_INITIALIZER( 1, -3,  2),
        FIX16_VEC3_INITIALIZER( 3,  1,  2),
        FIX16_VEC3_INITIALIZER( 3,  1, -2)
};

static const attribute_t _attributes_c[25] = {
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 }
};

static const polygon_t _polygons_c[25] = {
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 8, 25, 26, 19) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 4,  1, 12, 15) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(25,  4, 15, 26) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(24,  6,  3,  2) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 4,  6, 24,  1) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 2, 13, 23, 24) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 1, 24, 23, 12) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(17, 14,  3,  6) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(16,  5,  3, 14) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 3,  5,  2,  2) }, /* Triangle, see mesh_m.c */
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(13,  2,  5, 16) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(14, 14, 13, 16) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(23, 13, 14, 17) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(12, 23, 17, 15) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(17,  6,  9, 20) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(10, 21, 20,  9) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(11, 22, 21, 10) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 7, 11, 10,  9) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 4, 25,  9,  6) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(25,  8,  7,  9) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 8, 19, 18,  7) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(11,  7, 18, 22) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(26, 20, 18, 19) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(18, 20, 21, 22) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(15, 17, 20, 26) },
};

const mesh_t mesh_c = {
        .points         = _points_c,
        .points_count   = 27,
        .polygons       = _polygons_c,
        .attributes     = _attributes_c,
        .polygons_count = 25
};
