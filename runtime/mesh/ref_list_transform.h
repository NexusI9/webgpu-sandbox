#ifndef _MESH_REFERENCE_LIST_TRANSFORM_H_
#define _MESH_REFERENCE_LIST_TRANSFORM_H_

#include "../utils/vector/vector.h"
#include "ref_list.h"

// scale
void mesh_ref_list_scale(MeshRefList *, vec3);
void mesh_ref_list_scale_axis(MeshRefList *, vec3, const Axis);

// translate
void mesh_ref_list_translate(MeshRefList *, vec3);
void mesh_ref_list_translate_axis(MeshRefList *, vec3, const Axis);

// rotate
void mesh_ref_list_rotate(MeshRefList *, vec3);
void mesh_ref_list_rotate_axis(MeshRefList *, vec3, const Axis);

void mesh_ref_list_rotate_quat(MeshRefList *, versor);

void mesh_ref_list_lookat(MeshRefList *, vec3, vec3);

#endif
