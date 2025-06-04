#ifndef _GIZMO_CORE_H_
#define _GIZMO_CORE_H_

#include "../camera/camera.h"
#include "../mesh/mesh.h"
#include "../viewport/viewport.h"
#include <webgpu/webgpu.h>

#define GIZMO_LIGHT_SCALE 0.8


typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  Camera *camera;
  Viewport *viewport;
  MeshList *list;
} GizmoCreateDescriptor;

#endif
