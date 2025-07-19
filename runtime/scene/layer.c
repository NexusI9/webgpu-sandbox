#include "layer.h"

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
int scene_layer_create(SceneLayer *layer, scene_layer_name name,
                       size_t capacity) {

  return SCENE_LAYER_SUCCESS;
}

/**
   Empty layer mesh reference list, keep the name.
   Usually call during runtime for dynamic adjustment.
 */
void scene_layer_empty(SceneLayer *layer) {}

/**
   Free the whole layer (name and mesh reference list).
   Ususally call when destroying object owner.
 */
void scene_layer_free(SceneLayer *layer) {}

/**
   Insert a new mesh in the layer mesh reference list
 */
int scene_layer_insert(SceneLayer *layer, Mesh *mesh) {

  return SCENE_LAYER_SUCCESS;
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

  return SCENE_LAYER_SUCCESS;
}



/**
   Expand layer set
 */
int scene_layer_set_expand(SceneLayerSet * set){

  return 0;
}

/**
   create a new layer with a given name.
   Return the layer's reference or null if failed.
 */
SceneLayer *scene_layer_set_create_layer(SceneLayerSet *set,
                                         scene_layer_name name) {

  return NULL;
}

/**
   Find a layer in the set from its name. Use hashing function.
 */
SceneLayer *scene_layer_set_find(SceneLayerSet *set, scene_layer_name name) {

  return NULL;
}

/**
   Delete a layer from the set and free its content.
 */
int scene_layer_set_delete(SceneLayerSet *set, scene_layer_name name) {

  return SCENE_LAYER_SUCCESS;
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
  free(set->entries);
  set->capacity = 0;
  set->length = 0;
}
