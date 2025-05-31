#ifndef _GIZMO_CAMERA_H_
#define _GIZMO_CAMERA_H_

#include "../gizmo.h"
#include "../runtime/camera.h"
#include "../runtime/mesh.h"

typedef struct {
  Camera *camera;
  WGPUDevice *device;
  WGPUQueue *queue;
} GizmoCameraCreateDescriptor;

void gizmo_camera_create(Gizmo *, Gizmo *, const GizmoCameraCreateDescriptor *);

#endif
