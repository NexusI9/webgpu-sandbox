#ifndef _MESH_REFERENCE_LIST_TRANSFORM_H_
#define _MESH_REFERENCE_LIST_TRANSFORM_H_

#include "../utils/vector/vector.h"
#include "ref_list.h"

// scale
void mesh_ref_list_set_scale(MeshRefList *, vec3);
void mesh_ref_list_set_scale_axis(MeshRefList *, vec3, const Axis);

// translate
void mesh_ref_list_set_position(MeshRefList *, vec3);
void mesh_ref_list_set_position_axis(MeshRefList *, vec3, const Axis);

// rotate
void mesh_ref_list_set_rotation(MeshRefList *, vec3);
void mesh_ref_list_set_rotation_axis(MeshRefList *, vec3, const Axis);

void mesh_ref_list_set_rotation_quat(MeshRefList *, versor);

void mesh_ref_list_lookat(MeshRefList *, vec3, vec3);

#endif
