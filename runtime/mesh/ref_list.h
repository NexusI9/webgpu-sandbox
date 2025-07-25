#ifndef _MESH_REF_LIST_H_
#define _MESH_REF_LIST_H_
#include "core.h"

typedef struct {
  MeshRefList **lists;
  size_t length;
} MeshRefListArray;

// creators
MeshStatus mesh_ref_list_create(MeshRefList *, const size_t);
Mesh *mesh_ref_list_insert(MeshRefList *, Mesh *);

// destructors
void mesh_ref_list_remove(MeshRefList *, Mesh *);
void mesh_ref_list_empty(MeshRefList *);
void mesh_ref_list_free(MeshRefList *);

// transferts
MeshStatus mesh_ref_list_transfert(MeshRefList *, MeshRefList *);
MeshStatus mesh_ref_list_copy(const MeshRefList *, MeshRefList *);

// utils
Mesh *mesh_ref_list_find(const MeshRefList *, Mesh *);


// debug
void mesh_ref_list_print(MeshRefList *);
#endif
