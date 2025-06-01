#ifndef _MESH_LIST_H_
#define _MESH_LIST_H_
#include "./core.h"

typedef struct {
  struct Mesh *entries;
  size_t capacity;
  size_t length;
} MeshList;

int mesh_list_create(MeshList *, size_t);
Mesh *mesh_list_insert(MeshList *);

#endif
