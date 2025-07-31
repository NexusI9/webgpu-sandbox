#ifndef _MESH_TRANSFORM_H_
#define _MESH_TRANSFORM_H_

#include "../utils/vector/vector.h"
#include "core.h"

typedef void (*mesh_transform_axis_callback)(Mesh *, vec3, const Axis);
typedef void (*mesh_transform_uni_axis_callback)(Mesh *, vec3);
typedef void (*mesh_transform_callback)(Mesh *, vec3);

// scale
void mesh_scale(Mesh *, vec3);
void mesh_scale_axis(Mesh *, vec3, const Axis);
void mesh_scale_axis_add(Mesh *, vec3, const Axis);

// translate
void mesh_translate(Mesh *, vec3);
void mesh_translate_axis(Mesh *, vec3, const Axis);
void mesh_translate_axis_add(Mesh *, vec3, const Axis);

// rotate
void mesh_rotate(Mesh *, vec3);
void mesh_rotate_axis(Mesh *, vec3, const Axis);
void mesh_rotate_axis_add(Mesh *, vec3, const Axis);

void mesh_rotate_quat(Mesh *, versor);

void mesh_lookat(Mesh *, vec3, vec3);

#endif
