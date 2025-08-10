#include "list.h"
#include "../../utils/system.h"
#include "core.h"
#include "string.h"
#include <stdlib.h>

DynamicListStatus mesh_list_create(MeshList *list, size_t capacity) {

  return dyli_create((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(Mesh), capacity, "Mesh list");
}

Mesh *mesh_list_new_mesh(MeshList *list) {
  return (Mesh *)dyli_new_entry((void *)&list->entries, &list->capacity,
                                &list->length, sizeof(Mesh), "Mesh list");
}

void mesh_list_translate(MeshList *list, vec3 position) {
  for (size_t i = 0; i < list->length; i++)
    mesh_translate(&list->entries[i], position);
}
void mesh_list_rotate(MeshList *list, vec3 rotation) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate(&list->entries[i], rotation);
}
void mesh_list_rotate_quat(MeshList *list, versor quat) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate_quat(&list->entries[i], quat);
}
void mesh_list_scale(MeshList *list, vec3 scale) {
  for (size_t i = 0; i < list->length; i++)
    mesh_scale(&list->entries[i], scale);
}
