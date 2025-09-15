#include "ref_list.h"

#include <string.h>
#include <cglm/vec3.h>
#include <stdlib.h>

#include "core.h"
#include "utils/system.h"
#include "utils/dyli.h"

DynamicListStatus mesh_ref_list_create(MeshRefList *list,
                                       const size_t capacity) {

  return dyli_create((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(Mesh *), capacity, "Mesh reference list");
}

DynamicListStatus mesh_ref_list_array_create(MeshRefListArray *list_array,
                                             const size_t capacity) {

  return dyli_create((void *)&list_array->lists, &list_array->capacity,
                     &list_array->length, sizeof(MeshRefList *), capacity,
                     "Mesh reference list array");
}

Mesh *mesh_ref_list_insert(MeshRefList *list, Mesh *mesh) {

  if (dyli_insert((void *)&list->entries, &list->capacity, &list->length,
                  sizeof(Mesh *), (void *)&mesh, 1,
                  "Mesh Reference list") != DynamicListStatus_Success)
    return NULL;

  return mesh;
}

void mesh_ref_list_empty(MeshRefList *list) {
  dyli_empty((void *)list->entries, &list->length, sizeof(Mesh *));
}

void mesh_ref_list_free(MeshRefList *list) {
  dyli_free((void *)&list->entries, &list->capacity, &list->length);
}

/**
   Remove mesh from the selection.
   Use linear pointer comparison.
   TODO: Maybe for bigger selection, need a more efficient/quick way.
 */
DynamicListStatus mesh_ref_list_remove(MeshRefList *list, Mesh *mesh) {
  return dyli_remove((void *)list->entries, &list->length, sizeof(Mesh *),
                     (void *)&mesh, "Mesh reference list");
}

DynamicListStatus mesh_ref_list_remove_at_index(MeshRefList *list,
                                                size_t index) {
  return dyli_remove_at_index((void *)list->entries, &list->length,
                              sizeof(Mesh *), index, "Mesh reference list");
}

/**
   Linearily traverse the list and compare mesh id to find match
 */
Mesh *mesh_ref_list_find(const MeshRefList *list, const Mesh *mesh,
                         size_t *index) {

  for (size_t i = 0; i < list->length; i++)
    if (list->entries[i] == mesh) {
      if (index)
        *index = i;
      return list->entries[i];
    }

  return NULL;
}

/**
   Copy mesh pointers from one list to another
 */
MeshStatus mesh_ref_list_append(const MeshRefList *src, MeshRefList *dest,
                                   MeshRefList *exclude) {

  // expand if destination is too small
  while (dest->length + src->length >= dest->capacity) {
    size_t new_capacity = 2 * dest->capacity;
    Mesh **temp_entries =
        (Mesh **)realloc(dest->entries, new_capacity * sizeof(Mesh *));

    if (temp_entries) {
      dest->capacity = new_capacity;
      dest->entries = temp_entries;

    } else {
      VERBOSE_ERROR("Couldn't reallocate and expand mesh indexed list.");
      return MeshStatus_AllocFail;
    }
  }

  // if no exclude, simply mem copy directly
  if (exclude == NULL) {

    memcpy(&dest->entries[dest->length], src->entries,
           src->length * sizeof(Mesh *));

    dest->length += src->length;
  } else {
    // else need to check if the src mesh is not part of the exclude list before
    // inserting
    for (size_t i = 0; i < src->length; i++) {
      Mesh *src_mesh = src->entries[i];
      Mesh *find = mesh_ref_list_find(exclude, src_mesh, NULL);

      // skip if mesh pointer found in exclude list
      if (find != NULL)
        continue;

      // else insert in the destination list
      mesh_ref_list_insert(dest, src_mesh);
    }
  }

  return MeshStatus_Success;
}



/**
   Create a copy of a Gizmo Mesh list from a source to a given desination.
   It allocate memory for the new src.
 */
MeshStatus mesh_ref_list_create_and_copy(const MeshRefList *src, MeshRefList *dest) {

  if (src->capacity == 0 || src->entries == NULL)
    VERBOSE_ERROR(
        "Attempting to copy an unitialized list, entries: %p, capacity: %lu.",
        src->entries, src->capacity);

  // copy length
  dest->length = src->length;
  dest->capacity = src->capacity;
  dest->entries = malloc(dest->capacity * sizeof(Mesh *));

  if (dest->entries == NULL) {
    VERBOSE_ERROR("Couldn't allocate memory for mesh reference list copy.");
    dest->length = 0;
    return MeshStatus_AllocFail;
  }

  // copy meshes pointer
  memcpy(dest->entries, src->entries, dest->length * sizeof(Mesh *));
  return MeshStatus_Success;
}

void mesh_ref_list_print(MeshRefList *list) {

  for (size_t i = 0; i < list->length; i++)
    VERBOSE_DEBUG("[%p] %s", list->entries[i], list->entries[i]->name);
}

void mesh_ref_list_average_position(MeshRefList *list, vec3 *dest) {

  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, *dest);

  if (list->length == 0)
    return;

  for (size_t i = 0; i < list->length; i++)
    glm_vec3_add(list->entries[i]->position, *dest, *dest);

  glm_vec3_scale(*dest, 1.0f / list->length, *dest);
}

Mesh *mesh_ref_list_new_entry(MeshRefList *list) {
  return (Mesh *)dyli_new_entry((void *)&list->entries, &list->capacity,
                                &list->length, sizeof(Mesh *),
                                "Mesh reference list");
}

Mesh *mesh_ref_list_find_by_name(const MeshRefList *list, const char *name) {

  for (size_t i = 0; i < list->length; i++)
    if (strcmp(list->entries[i]->name, name) == 0)
      return list->entries[i];

  return NULL;
}
