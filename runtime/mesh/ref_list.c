#include "ref_list.h"
#include "../utils/system.h"
#include <string.h>

int mesh_reference_list_create(MeshRefList *list, size_t capacity) {

  list->entries = malloc(capacity * sizeof(Mesh));
  list->length = 0;
  list->capacity = capacity;

  if (list->entries == NULL) {
    perror("Couldn't allocate memory for mesh indexed list\n");
    return MESH_ALLOC_FAILURE;
  }

  return MESH_SUCCESS;
}

Mesh *mesh_reference_list_insert(MeshRefList *list, Mesh *mesh) {

  // ADD MESH TO LIST
  // eventually expand mesh vector if overflow
  if (list->length == list->capacity) {
    size_t new_capacity = list->capacity * 2;
    Mesh **temp = realloc(list->entries, sizeof(Mesh *) * new_capacity);

    if (temp) {
      list->entries = temp;
      list->capacity = new_capacity;
    } else {
      VERBOSE_PRINT("Scene mesh list reached full capacity, could not "
                    "reallocate new space\n");
      return NULL;
    }
  }

  list->entries[list->length] = mesh;
  list->length++;
  return mesh;
}

/**
   Copy mesh pointers from one list to another
 */
int mesh_reference_list_transfert(MeshRefList *src, MeshRefList *dest) {

  // expand if destination is too small
  if (dest->capacity < dest->length + src->length) {
    size_t new_capacity = dest->length + src->length;
    Mesh **temp_entries =
        (Mesh **)realloc(dest->entries, new_capacity * sizeof(Mesh *));

    if (temp_entries) {
      dest->capacity = new_capacity;
      dest->entries = temp_entries;

    } else {
      perror("Couldn't reallocate and expand mesh indexed list\n");

      return MESH_ALLOC_FAILURE;
    }
  }

  memcpy(&dest->entries[dest->length], src->entries,
         src->length * sizeof(Mesh *));

  dest->length += src->length;

  return MESH_SUCCESS;
}

/**
   Copy a Gizmo Mesh list from a source to a given desination
 */
int mesh_reference_list_copy(const MeshRefList *src, MeshRefList *dest) {

  // copy length
  dest->length = src->length;
  dest->capacity = src->capacity;
  dest->entries = malloc(dest->length * sizeof(Mesh *));

  if (dest->entries == NULL) {
    perror("Couldn't allocate memory for mesh reference list copy\n");
    dest->length = 0;
    return MESH_ALLOC_FAILURE;
  }

  // copy meshes pointer
  memcpy(dest->entries, src->entries, dest->length * sizeof(Mesh *));
  return MESH_SUCCESS;
}
