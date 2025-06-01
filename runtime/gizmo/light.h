#ifndef _GIZMO_LIGHT_H_
#define _GIZMO_LIGHT_H_

#include "../mesh.h"
#include "../shader.h"
#include "../viewport.h"

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  Camera *camera;
  Viewport *viewport;
  MeshList *list;
  MeshRefList *used_pointers;
} GizmoLightCreateDescriptor;

#endif
