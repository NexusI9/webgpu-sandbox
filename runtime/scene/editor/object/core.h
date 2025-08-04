#ifndef _SCENE_EDITOR_OBJECT_CORE_H_
#define _SCENE_EDITOR_OBJECT_CORE_H_

#include "../runtime/scene/editor/gizmo/gizmo.h"

typedef struct SceneEditorObject SceneEditorObject;

typedef void (*seo_transform_axis_callback)(SceneEditorObject *, vec3, const Axis);

struct SceneEditorObject {
  void *target;
  MeshRefList meshes;
  seo_transform_axis_callback transform_callback[GIZMO_TRANSFORM_MODE_COUNT];
};

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  Camera *camera;
  Viewport *viewport;
  MeshList *pool; // mesh pool from which gizmo mesh will be created
} SEOCreateDescriptor;

#endif
