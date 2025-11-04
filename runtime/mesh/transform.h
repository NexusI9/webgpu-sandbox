#ifndef _MESH_TRANSFORM_H_
#define _MESH_TRANSFORM_H_

#include <cglm/types.h>

#include "core.h"
#include "utils/vector/core.h"
#include "utils/vector/vector.h"

typedef void (*mesh_transform_axis_callback)(Mesh *, const vec3, const Axis);
typedef void (*mesh_transform_uni_axis_callback)(Mesh *, const vec3);
typedef void (*mesh_transform_callback)(Mesh *, const vec3);

EXTERN_C_BEGIN
// scale
void mesh_set_scale(Mesh *, const vec3);
void mesh_set_scale_axis(Mesh *, const vec3, const Axis);
void mesh_set_scale_axis_add(Mesh *, const vec3, const Axis);

// translate
void mesh_set_position(Mesh *, const vec3);
void mesh_set_position_axis(Mesh *, const vec3, const Axis);
void mesh_set_position_axis_add(Mesh *, const vec3, const Axis);

// rotate
void mesh_set_rotation(Mesh *, const vec3);
void mesh_set_rotation_axis(Mesh *, const vec3, const Axis);
void mesh_set_rotation_axis_add(Mesh *, const vec3, const Axis);

void mesh_set_rotation_quat(Mesh *, const versor);

void mesh_lookat(Mesh *, const vec3, const vec3);

EXTERN_C_END

#endif
