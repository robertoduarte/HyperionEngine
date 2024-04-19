#include "mesh.h"

#define COLOR1 RGB1555(1, 3, 25, 3)

static const fix16_vec3_t _points_i[9] = {
        FIX16_VEC3_INITIALIZER( 0,  0,  0),
        FIX16_VEC3_INITIALIZER( 1, -3,  2),
        FIX16_VEC3_INITIALIZER(-1, -1,  2),
        FIX16_VEC3_INITIALIZER(-1,  3,  2),
        FIX16_VEC3_INITIALIZER( 1,  3,  2),
        FIX16_VEC3_INITIALIZER( 1, -3, -2),
        FIX16_VEC3_INITIALIZER(-1, -1, -2),
        FIX16_VEC3_INITIALIZER(-1,  3, -2),
        FIX16_VEC3_INITIALIZER( 1,  3, -2)
};

static const attribute_t _attributes_i[6] = {
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 },
        { .draw_mode.raw = 0x00C0, .control.command = COMMAND_TYPE_POLYGON, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .palette_data.base_color = COLOR1 }
};

static const polygon_t _polygons_i[6] = {
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(4, 8, 7, 3) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(2, 6, 5, 1) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(3, 7, 6, 2) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(5, 6, 7, 8) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(3, 2, 1, 4) },
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, false), INDICES(5, 8, 4, 1) }
};

const mesh_t mesh_i = {
        .points         = _points_i,
        .points_count   = 9,
        .polygons       = _polygons_i,
        .attributes     = _attributes_i,
        .polygons_count = 6
};
