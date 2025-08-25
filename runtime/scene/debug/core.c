#include "core.h"
#include <stdint.h>

void scene_debug_init(SceneDebug *debug, const SceneDebugDescriptor *desc) {

  debug->camera = desc->camera;
  debug->viewport = desc->viewport;
  debug->pool = desc->pool;
  debug->queue = desc->queue;
  debug->device = desc->device;

  for (SceneDebugObject i = 0; i < SCENE_DEBUG_MESH_LIST_COUNT; i++)
    mesh_ref_list_create(&debug->object_list[i], MESH_REF_LIST_CAPACITY);
}

void scene_debug_destroy(SceneDebug *debug) {

  for (SceneDebugObject i = 0; i < SCENE_DEBUG_MESH_LIST_COUNT; i++)
    mesh_ref_list_free(&debug->object_list[i]);
}
