#ifndef _MESH_TRANSFORM_H_
#define _MESH_TRANSFORM_H_

#include "../utils/vector/vector.h"
#include "core.h"

typedef void (*mesh_transform_axis_callback)(Mesh *, const float);

// scale
void mesh_scale(Mesh *, vec3);
void mesh_scale_axis(Mesh *, const float, const Axis);
void mesh_scale_axis_add(Mesh *, const float, const Axis);

void mesh_scale_x(Mesh *, const float);
void mesh_scale_y(Mesh *, const float);
void mesh_scale_z(Mesh *, const float);

void mesh_scale_x_add(Mesh *, const float);
void mesh_scale_y_add(Mesh *, const float);
void mesh_scale_z_add(Mesh *, const float);

// translate
void mesh_translate(Mesh *, vec3);
void mesh_translate_axis(Mesh *, const float, const Axis);
void mesh_translate_axis_add(Mesh *, const float, const Axis);

void mesh_translate_x(Mesh *, const float);
void mesh_translate_y(Mesh *, const float);
void mesh_translate_z(Mesh *, const float);

void mesh_translate_x_add(Mesh *, const float);
void mesh_translate_y_add(Mesh *, const float);
void mesh_translate_z_add(Mesh *, const float);

// rotate
void mesh_rotate(Mesh *, vec3);
void mesh_rotate_axis(Mesh *, const float, const Axis);
void mesh_rotate_axis_add(Mesh *, const float, const Axis);

void mesh_rotate_x(Mesh *, const float);
void mesh_rotate_y(Mesh *, const float);
void mesh_rotate_z(Mesh *, const float);

void mesh_rotate_x_add(Mesh *, const float);
void mesh_rotate_y_add(Mesh *, const float);
void mesh_rotate_z_add(Mesh *, const float);

void mesh_rotate_quat(Mesh *, versor);

void mesh_lookat(Mesh *, vec3, vec3);

#endif
