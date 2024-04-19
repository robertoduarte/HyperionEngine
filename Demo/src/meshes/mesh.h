#ifndef MESHES_MESH_H
#define MESHES_MESH_H

#include <mic3d.h>

#define FLAGS(_sort_type, _plane_type, _use_texture)                           \
    .flags.sort_type   = _sort_type,                                           \
    .flags.plane_type  = _plane_type,                                          \
    .flags.use_texture = _use_texture

#define INDICES(a, b, c, d)                                                    \
    .indices.p0 = a,                                                           \
    .indices.p1 = b,                                                           \
    .indices.p2 = c,                                                           \
    .indices.p3 = d

#endif /* MESHES_MESH_H */
