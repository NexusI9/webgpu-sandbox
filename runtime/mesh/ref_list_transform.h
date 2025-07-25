#ifndef _MESH_REFERENCE_LIST_TRANSFORM_H_
#define _MESH_REFERENCE_LIST_TRANSFORM_H_

#include "ref_list.h"
#include "../utils/vector/vector.h"

// scale
void mesh_ref_list_scale(MeshRefList *, vec3);
void mesh_ref_list_scale_axis(MeshRefList *, const float, const Axis);
void mesh_ref_list_scale_axis_add(MeshRefList *, const float, const Axis);

void mesh_ref_list_scale_x(MeshRefList *, const float);
void mesh_ref_list_scale_y(MeshRefList *, const float);
void mesh_ref_list_scale_z(MeshRefList *, const float);

void mesh_ref_list_scale_x_add(MeshRefList *, const float);
void mesh_ref_list_scale_y_add(MeshRefList *, const float);
void mesh_ref_list_scale_z_add(MeshRefList *, const float);

// translate
void mesh_ref_list_translate(MeshRefList *, vec3);
void mesh_ref_list_translate_axis(MeshRefList *, const float, const Axis);
void mesh_ref_list_translate_axis_add(MeshRefList *, const float, const Axis);

void mesh_ref_list_translate_x(MeshRefList *, const float);
void mesh_ref_list_translate_y(MeshRefList *, const float);
void mesh_ref_list_translate_z(MeshRefList *, const float);

void mesh_ref_list_translate_x_add(MeshRefList *, const float);
void mesh_ref_list_translate_y_add(MeshRefList *, const float);
void mesh_ref_list_translate_z_add(MeshRefList *, const float);

// rotate
void mesh_ref_list_rotate(MeshRefList *, vec3);
void mesh_ref_list_rotate_axis(MeshRefList *, const float, const Axis);
void mesh_ref_list_rotate_axis_add(MeshRefList *, const float, const Axis);

void mesh_ref_list_rotate_x(MeshRefList *, const float);
void mesh_ref_list_rotate_y(MeshRefList *, const float);
void mesh_ref_list_rotate_z(MeshRefList *, const float);

void mesh_ref_list_rotate_x_add(MeshRefList *, const float);
void mesh_ref_list_rotate_y_add(MeshRefList *, const float);
void mesh_ref_list_rotate_z_add(MeshRefList *, const float);

void mesh_ref_list_rotate_quat(MeshRefList *, versor);

void mesh_ref_list_lookat(MeshRefList *, vec3, vec3);


#endif
