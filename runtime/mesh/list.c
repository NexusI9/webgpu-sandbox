#include "list.h"
#include "../../utils/system.h"


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

Mesh *mesh_list_insert(MeshList *list) {

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

