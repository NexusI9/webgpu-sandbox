#ifndef _SCENE_DEBUG_CORE_H_
#define _SCENE_DEBUG_CORE_H_

#include "../backend/ssbo.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#define SCENE_DEBUG_MESH_LIST_COUNT 2
#define SCENE_DEBUG_UNDEFINED 0

typedef enum {
  SceneDebugObject_Ray,
  SceneDebugObject_View,
} SceneDebugObject;

typedef struct {
  MeshList *pool;
  SSBOManager *ssbo;
  MeshRefList object_list[SCENE_DEBUG_MESH_LIST_COUNT];
  Camera *camera;
  Viewport *viewport;
  WGPUDevice device;
  WGPUQueue queue;
} SceneDebug;

typedef struct {
  MeshList *pool;
  SSBOManager *ssbo;
  Camera *camera;
  Viewport *viewport;
  const WGPUDevice device;
  const WGPUQueue queue;
} SceneDebugDescriptor;

void scene_debug_init(SceneDebug *, const SceneDebugDescriptor *);
void scene_debug_destroy(SceneDebug *);

#endif
