#ifndef _AO_BAKE_TEXTURE_LIST_H_
#define _AO_BAKE_TEXTURE_LIST_H_

#include <stddef.h>

#include "./core.h"
#include "../runtime/mesh/core.h"
#include "../runtime/texture/core.h"
#include "../utils/dyli.h"

DynamicListStatus ao_bake_texture_list_create(AOBakeTextureList *, size_t);

DynamicListStatus ao_bake_texture_list_insert(AOBakeTextureList *,
                                              AOBakeTextureListEntry *);

DynamicListStatus ao_bake_texture_list_remove(AOBakeTextureList *,
                                              AOBakeTextureListEntry *);

DynamicListStatus ao_bake_texture_list_destroy(AOBakeTextureList *,
                                               AOBakeTextureListEntry *);

AOBakeTextureListEntry *ao_bake_texture_list_new_entry(AOBakeTextureList *);

Texture *ao_bake_texture_list_find(AOBakeTextureList *, Mesh *, size_t *);

#endif
