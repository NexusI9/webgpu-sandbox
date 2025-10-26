#ifndef _SCENE_DEBUG_CORE_H_
#define _SCENE_DEBUG_CORE_H_

#include <webgpu/webgpu.h>

#include "backend/ubo.h"
#include "runtime/camera/camera.h"
#include "runtime/mesh/mesh.h"
#include "runtime/viewport/viewport.h"
#include "runtime/camera/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/list.h"
#include "runtime/viewport/core.h"

#define SCENE_DEBUG_MESH_LIST_COUNT 2
#define SCENE_DEBUG_UNDEFINED 0

typedef enum {
  SceneDebugObject_Ray,
  SceneDebugObject_View,
} SceneDebugObject;

typedef struct {
  MeshList *pool;
  UBOManager *ubo;
  MeshRefList object_list[SCENE_DEBUG_MESH_LIST_COUNT];
  Camera *camera;
  Viewport *viewport;

} SceneDebug;

typedef struct {
  MeshList *pool;
  UBOManager *ubo;
  Camera *camera;
  Viewport *viewport;

} SceneDebugDescriptor;

void scene_debug_init(SceneDebug *, const SceneDebugDescriptor *);
void scene_debug_destroy(SceneDebug *);

#endif
