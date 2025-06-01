#ifndef _GIZMO_H_
#define _GIZMO_H_

#include "../camera.h"
#include "../mesh.h"
#include "../viewport.h"
#include <webgpu/webgpu.h>

#define GIZMO_LIGHT_SCALE 0.8;

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  Camera *camera;
  Viewport *viewport;
  MeshList *list;
} GizmoCreateDescriptor;

#endif
