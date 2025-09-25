#ifndef _MESH_LIST_H_
#define _MESH_LIST_H_
#include <cglm/types.h>
#include <stddef.h>

#include "./core.h"
#include "transform.h"

typedef struct {
  struct Mesh *entries;
  size_t capacity;
  size_t length;
} MeshList;

static inline void mesh_list_set_position(MeshList *list, vec3 position) {
  for (size_t i = 0; i < list->length; i++)
    mesh_set_position(&list->entries[i], position);
}

static inline void mesh_list_set_rotation(MeshList *list, vec3 rotation) {
  for (size_t i = 0; i < list->length; i++)
    mesh_set_rotation(&list->entries[i], rotation);
}
static inline void mesh_list_set_rotation_quat(MeshList *list, versor quat) {
  for (size_t i = 0; i < list->length; i++)
    mesh_set_rotation_quat(&list->entries[i], quat);
}
static inline void mesh_list_set_scale(MeshList *list, vec3 scale) {
  for (size_t i = 0; i < list->length; i++)
    mesh_set_scale(&list->entries[i], scale);
}

#endif
