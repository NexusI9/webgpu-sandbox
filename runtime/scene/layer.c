#include "layer.h"
#include "../../utils/dyli.h"
#include "../../utils/hash.h"
#include "../../utils/system.h"
#include <string.h>

/*
   ▗▖    ▗▄▖▗▖  ▗▖▗▄▄▄▖▗▄▄▖
   ▐▌   ▐▌ ▐▌▝▚▞▘ ▐▌   ▐▌ ▐▌
   ▐▌   ▐▛▀▜▌ ▐▌  ▐▛▀▀▘▐▛▀▚▖
   ▐▙▄▄▖▐▌ ▐▌ ▐▌  ▐▙▄▄▖▐▌ ▐▌

 */

/**
   Create a new layer by duplicating name and allocating room for its mesh
   reference list.
 */
int scene_layer_create(SceneLayer *layer, const char *name, size_t capacity) {

  // assign name
  layer->name = strdup(name);

  // alloc mesh ref list
  mesh_reference_list_create(&layer->meshes, capacity);

  return SCENE_LAYER_SUCCESS;
}

/**
   Empty layer mesh reference list, keep the name.
   Usually call during runtime for dynamic adjustment.
 */
void scene_layer_empty(SceneLayer *layer) {
  mesh_reference_list_empty(&layer->meshes);
}

/**
   Free the whole layer (name and mesh reference list).
   Ususally call when destroying object owner.
 */
void scene_layer_free(SceneLayer *layer) {

  // free layer name
  free(layer->name);
  layer->name = NULL;

  // free mesh ref list
  mesh_reference_list_free(&layer->meshes);
}

/**
   Insert a new mesh in the layer mesh reference list
 */
Mesh *scene_layer_insert(SceneLayer *layer, Mesh *mesh) {
  return mesh_reference_list_insert(&layer->meshes, mesh);
}

/*
   ▗▖    ▗▄▖▗▖  ▗▖▗▄▄▄▖▗▄▄▖      ▗▄▄▖▗▄▄▄▖▗▄▄▄▖
   ▐▌   ▐▌ ▐▌▝▚▞▘ ▐▌   ▐▌ ▐▌    ▐▌   ▐▌     █
   ▐▌   ▐▛▀▜▌ ▐▌  ▐▛▀▀▘▐▛▀▚▖     ▝▀▚▖▐▛▀▀▘  █
   ▐▙▄▄▖▐▌ ▐▌ ▐▌  ▐▙▄▄▖▐▌ ▐▌    ▗▄▄▞▘▐▙▄▄▖  █

 */

static int scene_layer_set_expand(SceneLayerSet *);

/**
   Create a new layer set with certain capacity
 */
int scene_layer_set_create(SceneLayerSet *set, size_t capacity) {

  return dyli_create((void *)&set->entries, &set->capacity, &set->length,
                     sizeof(SceneLayer), SCENE_LAYER_SET_CAPACITY,
                     "Scene layer set");
}

/**
   Expand layer set
 */
int scene_layer_set_expand(SceneLayerSet *set) {

  return dyli_expand((void *)&set->entries, &set->capacity, &set->length,
                     sizeof(SceneLayer), 2, "Scene layer set");
}

/**
   create a new layer with a given name.
   Return the layer's reference or null if failed.
 */
SceneLayer *scene_layer_set_create_layer(SceneLayerSet *set, const char *name) {

  if (set->length >= set->capacity * 0.75 &&
      dyli_expand((void *)&set->entries, &set->capacity, &set->length,
                  sizeof(SceneLayer), 2,
                  "Scene Layer Set") != DYNAMIC_LIST_SUCCESS) {
    return NULL;
  }

  hash_djb2_t hash = hash_djb2(name) % set->capacity;

  // init new layer
  SceneLayer *layer = &set->entries[hash];

  // linear prob
  hash_djb2_t init_hash = hash;
  while (true) {
    // if not occupied or names don't match(collision)
    if (layer->meshes.entries == NULL || strcmp(layer->name, name) == 0)
      break;

    // else increment hash
    hash = (hash + 1) % set->capacity;
    if (hash == init_hash)
      return NULL;

    layer = &set->entries[hash];
  }

  // create new scene layer
  scene_layer_create(layer, name, SCENE_LAYER_CAPACITY);

  return layer;
}

/**
   Find a layer in the set from its name. Use hashing function.
 */
SceneLayer *scene_layer_set_find(SceneLayerSet *set, const char *name) {

  hash_djb2_t hash = hash_djb2(name) % set->capacity;

  hash_djb2_t init_hash = hash;

  while (true) {

    SceneLayer *layer = &set->entries[hash];

    // if empty slot
    if (layer->meshes.entries == NULL || layer->name == NULL)
      return NULL;

    // if name match key
    if (strcmp(layer->name, name) == 0)
      return layer;

    // probing
    hash = (hash + 1) % set->capacity;
    if (init_hash == hash)
      return NULL;
  }

  return NULL;
}

/**
   Delete a layer from the set and free its content.
 */
int scene_layer_set_delete(SceneLayerSet *set, const char *name) {

  hash_djb2_t hash = hash_djb2(name) % set->capacity;

  SceneLayer *layer = scene_layer_set_find(set, name);

  if (layer && layer->meshes.entries != NULL) {
    scene_layer_free(layer);
    set->length--;
    return SCENE_LAYER_SUCCESS;
  }

  return SCENE_LAYER_SET_UNFOUND;
}

/**
   Clean up all layers and free the whole set.
   Used during owner uninstanciation/ deletion from memory.
 */
void scene_layer_set_free(SceneLayerSet *set) {

  // free layers
  for (size_t l = 0; l < set->length; l++)
    scene_layer_free(&set->entries[l]);

  // free set
  dyli_free((void *)&set->entries, &set->capacity, &set->length);
}

/**
   Allow to directly insert mesh in a given layer from a layer set.
   If the layer is not found it create a new one with the given name.
 */
Mesh *scene_layer_set_insert_mesh(SceneLayerSet *set, const char *name,
                                  Mesh *mesh) {

  SceneLayer *layer = scene_layer_set_find(set, name);

  // create layer if not found
  if (layer == NULL)
    layer = scene_layer_set_create_layer(set, name);

  // if layer creationg fail, finally return NULL object
  if (layer == NULL) {
    VERBOSE_ERROR("Couldn't insert new mesh in scene layer: %s\n", name);
    return NULL;
  }

  // else insert mesh in layer
  return scene_layer_insert(layer, mesh);
}
