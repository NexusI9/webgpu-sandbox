#ifndef _GIZMO_CAMERA_H_
#define _GIZMO_CAMERA_H_

#include "../runtime/camera.h"
#include "../runtime/mesh.h"

typedef struct {
  Camera *target;
  MeshRefList meshes;
} GizmoCamera;

typedef struct {
  Camera *camera;
  WGPUDevice *device;
  WGPUQueue *queue;
} GizmoCameraCreateDescriptor;

void gizmo_camera_create(Mesh *, Mesh *, const GizmoCameraCreateDescriptor *);

#endif
