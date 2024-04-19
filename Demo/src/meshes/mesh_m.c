#include "mesh.h"

#define COLOR1 RGB1555(1, 25, 3, 3)

static const fix16_vec3_t _points_m[29] = {
        FIX16_VEC3_INITIALIZER( 0,  0,  0),
        FIX16_VEC3_INITIALIZER( 5, -3,  2),
        FIX16_VEC3_INITIALIZER( 3, -3,  2),
        FIX16_VEC3_INITIALIZER( 3,  3,  2),
        FIX16_VEC3_INITIALIZER( 5,  3,  2),
        FIX16_VEC3_INITIALIZER( 5, -3, -2),
        FIX16_VEC3_INITIALIZER(-3, -3, -2),
        FIX16_VEC3_INITIALIZER(-3, -3,  2),//7
        FIX16_VEC3_INITIALIZER(-3, -1,  2),//8
        FIX16_VEC3_INITIALIZER( 3, -1,  2),
        FIX16_VEC3_INITIALIZER(-5, -1,  2),//10
        FIX16_VEC3_INITIALIZER(-5, -1, -2),
        FIX16_VEC3_INITIALIZER(-5,  3, -2),
        FIX16_VEC3_INITIALIZER(-5,  3,  2),
        FIX16_VEC3_INITIALIZER(-3,  3,  2),
        //
        FIX16_VEC3_INITIALIZER(-3,  3, -2),
        FIX16_VEC3_INITIALIZER(-1, -1,  2),
        FIX16_VEC3_INITIALIZER(-1, -1, -2),
        FIX16_VEC3_INITIALIZER(-1,  3, -2),
        FIX16_VEC3_INITIALIZER(-1,  3,  2),
        FIX16_VEC3_INITIALIZER( 1,  3,  2),
        FIX16_VEC3_INITIALIZER( 1,  3, -2),
        FIX16_VEC3_INITIALIZER( 1, -1,  2),
        FIX16_VEC3_INITIALIZER( 3, -1, -2),
        FIX16_VEC3_INITIALIZER( 3,  3, -2),
        FIX16_VEC3_INITIALIZER( 5,  3, -2),
        FIX16_VEC3_INITIALIZER( 3, -3, -2),
        FIX16_VEC3_INITIALIZER(-3, -1, -2),
        FIX16_VEC3_INITIALIZER( 1, -1, -2)
};

static const attribute_t _attributes_m[23] = {
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

static const polygon_t _polygons_m[23] = {
        /* Polygon #0 is a triangle and originally, its indices were
         *         ( 8,  8, 10,  7).
         *           ^   ^   ^   ^
         *          i0  i1  i2  i3
         *
         * Mic3D however does: (i1-i0)x(i2-i0), which would cull this polygon.
         * The solution is then to instead "merge" the 3rd and 4th points
         * together */
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(  8, 10,  7,  7) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 11, 27,  6,  6) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(  5,  1,  7,  6) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(  7,  2,  9,  8) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(  6,  7, 10, 11) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 11, 10, 13, 12) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 13, 14, 15, 12) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 10,  8, 14, 13) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 17, 16, 19, 18) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 19, 20, 21, 18) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 16, 22, 20, 19) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 23,  9,  3, 24) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(  3,  4, 25, 24) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(  1,  5, 25,  4) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 25,  5, 26, 24) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 23, 26,  6, 27) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(  8, 27, 15, 14) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 15, 27, 11, 12) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 16, 17, 27,  8) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 22, 28, 21, 20) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES( 21, 28, 17, 18) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(  9, 23, 28, 22) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(  2,  1,  4,  3) }
};

const mesh_t mesh_m = {
        .points         = _points_m,
        .points_count   = 29,
        .polygons       = _polygons_m,
        .attributes     = _attributes_m,
        .polygons_count = 23
};
