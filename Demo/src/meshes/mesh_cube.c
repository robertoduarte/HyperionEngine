#include "mesh.h"

static const fix16_vec3_t _points[] = {
        FIX16_VEC3_INITIALIZER(-10.0, -10.0,  10.0), /* 0 */
        FIX16_VEC3_INITIALIZER( 10.0, -10.0,  10.0), /* 1 */
        FIX16_VEC3_INITIALIZER( 10.0,  10.0,  10.0), /* 2 */
        FIX16_VEC3_INITIALIZER(-10.0,  10.0,  10.0), /* 3 */
        FIX16_VEC3_INITIALIZER(-10.0, -10.0, -10.0), /* 4 */
        FIX16_VEC3_INITIALIZER( 10.0, -10.0, -10.0), /* 5 */
        FIX16_VEC3_INITIALIZER( 10.0,  10.0, -10.0), /* 6 */
        FIX16_VEC3_INITIALIZER(-10.0,  10.0, -10.0)  /* 7 */
};

static const fix16_vec3_t _normals[] = {
        FIX16_VEC3_INITIALIZER(0.0, 0.0, 1.0),
        FIX16_VEC3_INITIALIZER(0.0, 0.0, 1.0),
        FIX16_VEC3_INITIALIZER(0.0, 0.0, 1.0),
        FIX16_VEC3_INITIALIZER(0.0, 0.0, 1.0),
        FIX16_VEC3_INITIALIZER(0.0, 0.0, 1.0),
        FIX16_VEC3_INITIALIZER(0.0, 0.0, 1.0),
        FIX16_VEC3_INITIALIZER(0.0, 0.0, 1.0),
        FIX16_VEC3_INITIALIZER(0.0, 0.0, 1.0)
};

static const attribute_t _attributes[] = {
        { .draw_mode.color_mode = VDP1_CMDT_CM_RGB_32768,                                        .control.link_type = LINK_TYPE_JUMP_ASSIGN, .control.command = COMMAND_TYPE_DISTORTED_SPRITE, .texture_slot = 0 },
        { .draw_mode.color_mode = VDP1_CMDT_CM_RGB_32768,                                        .control.link_type = LINK_TYPE_JUMP_ASSIGN, .control.command = COMMAND_TYPE_DISTORTED_SPRITE, .texture_slot = 1 },
        { .draw_mode.color_mode = VDP1_CMDT_CM_CB_256,    .draw_mode.trans_pixel_disable = true, .control.link_type = LINK_TYPE_JUMP_ASSIGN, .control.command = COMMAND_TYPE_DISTORTED_SPRITE, .texture_slot = 2 },
        { .draw_mode.color_mode = VDP1_CMDT_CM_RGB_32768,                                        .control.link_type = LINK_TYPE_JUMP_ASSIGN, .control.command = COMMAND_TYPE_DISTORTED_SPRITE, .texture_slot = 0 },
        { .draw_mode.color_mode = VDP1_CMDT_CM_RGB_32768,                                        .control.link_type = LINK_TYPE_JUMP_ASSIGN, .control.command = COMMAND_TYPE_DISTORTED_SPRITE, .texture_slot = 1 },
        { .draw_mode.color_mode = VDP1_CMDT_CM_CB_256,                                           .control.link_type = LINK_TYPE_JUMP_ASSIGN, .control.command = COMMAND_TYPE_DISTORTED_SPRITE, .texture_slot = 2 }
};

static const polygon_t _polygons[] = {
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, true), INDICES(0, 1, 2, 3) }, /* Back */
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, true), INDICES(4, 0, 3, 7) }, /* Left */
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, true), INDICES(5, 4, 7, 6) }, /* Front */
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, true), INDICES(1, 5, 6, 2) }, /* Right */
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, true), INDICES(4, 5, 1, 0) }, /* Top */
        { FLAGS(SORT_TYPE_CENTER, PLANE_TYPE_SINGLE, true), INDICES(3, 2, 6, 7) }  /* Bottom */
};

const mesh_t mesh_cube = {
        .points         = _points,
        .points_count   = 8,
        .polygons       = _polygons,
        .normals        = _normals,
        .attributes     = _attributes,
        .polygons_count = 6
};
