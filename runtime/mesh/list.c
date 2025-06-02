#include "list.h"
#include "../../utils/system.h"
#include "core.h"

int mesh_list_create(MeshList *list, size_t capacity) {

  list->entries = malloc(capacity * sizeof(Mesh));
  if (list->entries == NULL) {
    VERBOSE_WARNING("Cannot create new mesh list");
    return 1;
  }

  list->length = 0;
  list->capacity = capacity;

  return 0;
}

Mesh *mesh_list_new_mesh(MeshList *list) {

  if (list->length == list->capacity) {
    size_t new_capacity = list->capacity * 2;
    Mesh *temp = realloc(list->entries, sizeof(Mesh) * new_capacity);

    if (temp) {
      list->entries = temp;
      list->capacity = new_capacity;
    } else {
      VERBOSE_WARNING("Scene mesh list reached full capacity, could not "
                      "reallocate new space\n");
      return 0;
    }
  }

  return &list->entries[list->length++];
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
