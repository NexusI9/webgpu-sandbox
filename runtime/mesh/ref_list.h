#ifndef _MESH_REF_LIST_H_
#define _MESH_REF_LIST_H_
#include "../utils/dyli.h"
#include "core.h"

#define MESH_REF_LIST_CAPACITY 24
#define MESH_REF_LIST_ARRAY_CAPACITY 6

typedef struct {
  MeshRefList **lists;
  size_t capacity;
  size_t length;
} MeshRefListArray;

// creators
DynamicListStatus mesh_ref_list_create(MeshRefList *, const size_t);
DynamicListStatus mesh_ref_list_array_create(MeshRefListArray *, const size_t);
Mesh *mesh_ref_list_insert(MeshRefList *, Mesh *);
Mesh *mesh_ref_list_new_entry(MeshRefList *);

// destructors
DynamicListStatus mesh_ref_list_remove(MeshRefList *, Mesh *);
DynamicListStatus mesh_ref_list_remove_at_index(MeshRefList *, size_t);
void mesh_ref_list_empty(MeshRefList *);
void mesh_ref_list_free(MeshRefList *);
Mesh *mesh_ref_list_find_by_name(const MeshRefList *, const char *);

// transferts
MeshStatus mesh_ref_list_transfert(const MeshRefList *, MeshRefList *,
                                   MeshRefList *);
MeshStatus mesh_ref_list_copy(const MeshRefList *, MeshRefList *);

// utils
Mesh *mesh_ref_list_find(const MeshRefList *, Mesh *, size_t *);

// debug
void mesh_ref_list_print(MeshRefList *);

void mesh_ref_list_average_position(MeshRefList *, vec3 *);
#endif
