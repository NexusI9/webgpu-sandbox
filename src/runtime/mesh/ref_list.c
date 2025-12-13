#include "ref_list.h"

#include <cglm/vec3.h>
#include <stdlib.h>
#include <string.h>

#include "backend/logger.h"
#include "core.h"
#include "utils/dyli.h"
#include "utils/stli.h"

DynamicListStatus mesh_ref_list_create(MeshRefList *list,
                                       const size_t capacity) {

  return dyli_create((void *)&list->entries, &list->capacity, &list->count,
                     sizeof(Mesh *), capacity, "Mesh reference list");
}

Mesh *mesh_ref_list_insert(MeshRefList *list, Mesh *mesh) {

  if (dyli_insert((void *)&list->entries, &list->capacity, &list->count,
                  sizeof(Mesh *), (void *)&mesh, 1,
                  "Mesh Reference list") != DynamicListStatus_Success)
    return NULL;

  return mesh;
}

void mesh_ref_list_empty(MeshRefList *list) {
  dyli_empty((void *)list->entries, &list->count, sizeof(Mesh *));
}

void mesh_ref_list_free(MeshRefList *list) {
  dyli_free((void *)&list->entries, &list->capacity, &list->count);
}

/**
   Remove mesh from the selection.
   Use linear pointer comparison.
   TODO: Maybe for bigger selection, need a more efficient/quick way.
 */
DynamicListStatus mesh_ref_list_remove(MeshRefList *list, Mesh *mesh) {
  return dyli_remove((void *)list->entries, &list->count, sizeof(Mesh *),
                     (void *)&mesh, "Mesh reference list");
}

DynamicListStatus mesh_ref_list_remove_at_index(MeshRefList *list,
                                                size_t index) {
  return dyli_remove_at_index((void *)list->entries, &list->count,
                              sizeof(Mesh *), index, "Mesh reference list");
}

/**
   Linearily traverse the list and compare mesh id to find match
 */
Mesh *mesh_ref_list_find(const MeshRefList *list, const Mesh *mesh,
                         size_t *index) {

  for (size_t i = 0; i < list->count; i++)
    if (list->entries[i] == mesh) {
      if (index)
        *index = i;
      return list->entries[i];
    }

  if (index)
    *index = MESH_REF_LIST_UNFOUND_ENTRY;

  return NULL;
}

/**
   Copy mesh pointers from one list to another
 */
MeshStatus mesh_ref_list_append(const MeshRefList *src, MeshRefList *dest,
                                MeshRefList *exclude) {

  // expand if destination is too small
  while (dest->count + src->count >= dest->capacity) {
    size_t new_capacity = 2 * dest->capacity;
    Mesh **temp_entries =
        (Mesh **)realloc(dest->entries, new_capacity * sizeof(Mesh *));

    if (temp_entries) {
      dest->capacity = new_capacity;
      dest->entries = temp_entries;

    } else {
      logger_add(LoggerFlag_Error,
                 "Couldn't reallocate and expand mesh indexed list.");
      return MeshStatus_AllocFail;
    }
  }

  // if no exclude, simply mem copy directly
  if (exclude == NULL) {

    memcpy(&dest->entries[dest->count], src->entries,
           src->count * sizeof(Mesh *));

    dest->count += src->count;
  } else {
    // else need to check if the src mesh is not part of the exclude list before
    // inserting
    for (size_t i = 0; i < src->count; i++) {
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
MeshStatus mesh_ref_list_create_and_copy(const MeshRefList *src,
                                         MeshRefList *dest) {

  if (src->capacity == 0 || src->entries == NULL)
    logger_add(
        LoggerFlag_Error,
        "Attempting to copy an unitialized list, entries: %p, capacity: %lu.",
        src->entries, src->capacity);

  // copy count
  dest->count = src->count;
  dest->capacity = src->capacity;
  dest->entries = malloc(dest->capacity * sizeof(Mesh *));

  if (dest->entries == NULL) {
    logger_add(LoggerFlag_Error,
               "Couldn't allocate memory for mesh reference list copy.");
    dest->count = 0;
    return MeshStatus_AllocFail;
  }

  // copy meshes pointer
  memcpy(dest->entries, src->entries, dest->count * sizeof(Mesh *));
  return MeshStatus_Success;
}

void mesh_ref_list_print(MeshRefList *list) {

  for (size_t i = 0; i < list->count; i++)
    logger_add(LoggerFlag_Debug, "[%p] %s", list->entries[i],
               list->entries[i]->name);
}

void mesh_ref_list_average_position(MeshRefList *list, vec3 *dest) {

  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, *dest);

  if (list->count == 0)
    return;

  for (size_t i = 0; i < list->count; i++)
    glm_vec3_add(list->entries[i]->position, *dest, *dest);

  glm_vec3_scale(*dest, 1.0f / list->count, *dest);
}

Mesh *mesh_ref_list_new_entry(MeshRefList *list) {
  return (Mesh *)dyli_new_entry((void *)&list->entries, &list->capacity,
                                &list->count, sizeof(Mesh *),
                                "Mesh reference list");
}

Mesh *mesh_ref_list_find_by_name(const MeshRefList *list, const char *name) {

  for (size_t i = 0; i < list->count; i++)
    if (strcmp(list->entries[i]->name, name) == 0)
      return list->entries[i];

  return NULL;
}

StaticListStatus mesh_ref_list_array_create(MeshRefListArray *list_array) {

  return stli_create(&list_array->capacity, &list_array->count,
                     MESH_REF_LIST_CAPACITY, "Mesh reference list array");
}

StaticListStatus mesh_ref_list_array_copy(const MeshRefListArray *src,
                                          MeshRefListArray *dest) {

  memcpy(dest->lists, src->lists, src->count * sizeof(MeshRefList *));

  dest->count = src->count;
  dest->capacity = src->capacity;

  return StaticListStatus_Success;
}

StaticListStatus mesh_ref_list_array_destroy(MeshRefListArray *list) {

  list->capacity = 0;
  list->count = 0;

  return StaticListStatus_Success;
}

StaticListStatus mesh_ref_list_array_insert(MeshRefListArray *array,
                                            MeshRefList *list) {

  return stli_insert((void *)array->lists, array->capacity, &array->count,
                     sizeof(MeshRefList *), &list, "Mesh reference list array");
}
