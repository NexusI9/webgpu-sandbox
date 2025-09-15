#include "texture_list.h"

#include "core.h"
#include "../runtime/mesh/core.h"
#include "../runtime/texture/core.h"
#include "../utils/dyli.h"

DynamicListStatus ao_bake_texture_list_create(AOBakeTextureList *list,
                                              size_t capacity) {
  return dyli_create((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(AOBakeTextureListEntry), capacity,
                     "AO Texture List");
}

DynamicListStatus ao_bake_texture_list_insert(AOBakeTextureList *list,
                                              AOBakeTextureListEntry *entry) {
  return dyli_insert((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(AOBakeTextureListEntry), (void *)entry, 1,
                     "AO Texture List");
}

AOBakeTextureListEntry *
ao_bake_texture_list_new_entry(AOBakeTextureList *list) {
  return dyli_new_entry((void *)&list->entries, &list->capacity, &list->length,
                        sizeof(AOBakeTextureListEntry), "AO Texture List");
}

DynamicListStatus ao_bake_texture_list_remove(AOBakeTextureList *list,
                                              AOBakeTextureListEntry *entry) {
  return dyli_remove((void *)&list->entries, &list->length,
                     sizeof(AOBakeTextureListEntry), (void *)entry,
                     "AO Texture List");
}

DynamicListStatus ao_bake_texture_list_destroy(AOBakeTextureList *list,
                                               AOBakeTextureListEntry *entry) {
  return dyli_free((void *)&list->entries, &list->capacity, &list->length);
}

Texture *ao_bake_texture_list_find(AOBakeTextureList *list, Mesh *mesh,
                                   size_t *index) {
  for (size_t i = 0; i < list->length; i++)
    if (list->entries[i].owner == mesh) {
      if (index)
        *index = i;
      return &list->entries[i].texture;
    }

  if (index)
    *index = DYLI_INVALID_INDEX;

  return NULL;
}
