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


static inline DynamicListStatus mesh_list_create(MeshList *list, size_t capacity) {

  return dyli_create((void **)&list->entries, &list->capacity, &list->length,
                     sizeof(Mesh), capacity, "Mesh list");
}

static inline Mesh *mesh_list_new_mesh(MeshList *list) {
  return (Mesh *)dyli_new_entry((void **)&list->entries, &list->capacity,
                                &list->length, sizeof(Mesh), "Mesh list");
}


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
