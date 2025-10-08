#ifndef _MESH_TRANSFORM_H_
#define _MESH_TRANSFORM_H_

#include <cglm/types.h>

#include "core.h"
#include "utils/vector/core.h"
#include "utils/vector/vector.h"

typedef void (*mesh_transform_axis_callback)(Mesh *, vec3, const Axis);
typedef void (*mesh_transform_uni_axis_callback)(Mesh *, vec3);
typedef void (*mesh_transform_callback)(Mesh *, vec3);

#ifdef __cplusplus
extern "C" {
#endif

// scale
void mesh_set_scale(Mesh *, vec3);
void mesh_set_scale_axis(Mesh *, vec3, const Axis);
void mesh_set_scale_axis_add(Mesh *, vec3, const Axis);

// translate
void mesh_set_position(Mesh *, vec3);
void mesh_set_position_axis(Mesh *, vec3, const Axis);
void mesh_set_position_axis_add(Mesh *, vec3, const Axis);

// rotate
void mesh_set_rotation(Mesh *, vec3);
void mesh_set_rotation_axis(Mesh *, vec3, const Axis);
void mesh_set_rotation_axis_add(Mesh *, vec3, const Axis);

void mesh_set_rotation_quat(Mesh *, versor);

void mesh_lookat(Mesh *, vec3, vec3);

#ifdef __cplusplus
}
#endif

#endif
