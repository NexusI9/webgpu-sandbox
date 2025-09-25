#include "list.h"
#include "utils/dyli.h"

DynamicListStatus mesh_list_create(MeshList *list, size_t capacity) {

  return dyli_create((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(Mesh), capacity, "Mesh list");
}

Mesh *mesh_list_new_mesh(MeshList *list) {
  return (Mesh *)dyli_new_entry((void *)&list->entries, &list->capacity,
                                &list->length, sizeof(Mesh), "Mesh list");
}

