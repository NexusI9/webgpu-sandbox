#ifndef _MESH_REF_LIST_H_
#define _MESH_REF_LIST_H_
#include "core.h"

int mesh_reference_list_create(MeshRefList *, size_t);
Mesh *mesh_reference_list_insert(MeshRefList *, Mesh *);
int mesh_reference_list_transfert(MeshRefList *, MeshRefList *);
int mesh_reference_list_copy(const MeshRefList *, MeshRefList *);


#endif
