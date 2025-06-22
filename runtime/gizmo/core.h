#ifndef _GIZMO_CORE_H_
#define _GIZMO_CORE_H_

#include "../camera/camera.h"
#include "../mesh/mesh.h"
#include "../viewport/viewport.h"
#include <webgpu/webgpu.h>

#define GIZMO_BILLBOARD_SCALE ((vec3){0.85f, 0.85f, 0.85f})

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  Camera *camera;
  Viewport *viewport;
  MeshList *list;
} GizmoCreateDescriptor;

#endif
