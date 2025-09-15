#ifndef _MESH_LIST_H_
#define _MESH_LIST_H_
#include <cglm/types.h>
#include <stddef.h>

#include "./core.h"
#include "../utils/dyli.h"

typedef struct {
  struct Mesh *entries;
  size_t capacity;
  size_t length;
} MeshList;

DynamicListStatus mesh_list_create(MeshList *, size_t);
Mesh *mesh_list_new_mesh(MeshList *);

void mesh_list_set_position(MeshList *, vec3);
void mesh_list_set_rotation(MeshList *, vec3);
void mesh_list_set_rotation_quat(MeshList *, versor);
void mesh_list_set_scale(MeshList *, vec3);

#endif
