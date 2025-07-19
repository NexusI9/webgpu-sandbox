#ifndef _MESH_REF_LIST_H_
#define _MESH_REF_LIST_H_
#include "core.h"

// creators
int mesh_reference_list_create(MeshRefList *, const size_t);
Mesh *mesh_reference_list_insert(MeshRefList *, Mesh *);

// destructors
void mesh_reference_list_remove(MeshRefList *, Mesh *);
void mesh_reference_list_empty(MeshRefList *);
void mesh_reference_list_free(MeshRefList *);

// transferts
int mesh_reference_list_transfert(MeshRefList *, MeshRefList *);
int mesh_reference_list_copy(const MeshRefList *, MeshRefList *);

// utils
Mesh *mesh_reference_list_find(const MeshRefList *, Mesh *);

// transform
void mesh_reference_list_translate(MeshRefList *, vec3);
void mesh_reference_list_rotate(MeshRefList *, vec3);
void mesh_reference_list_rotate_quat(MeshRefList *, versor);
void mesh_reference_list_scale(MeshRefList *, vec3);

// debug
void mesh_reference_list_print(MeshRefList *);
#endif
