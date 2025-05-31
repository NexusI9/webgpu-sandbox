#ifndef _GIZMO_H_
#define _GIZMO_H_

#include "../runtime/mesh.h"
#include <cglm/cglm.h>
#include <webgpu/webgpu.h>

typedef struct {
  Mesh *mesh;
  vec3 *position;
  vec3 *scale;
  WGPUDevice *device;
  WGPUQueue *queue;
  const char *label;
  const char *texture_path;
  Camera *camera;
  Viewport *viewport;
} GizmoCreateBillboardDescriptor;

void gizmo_create_billboard(const GizmoCreateBillboardDescriptor *);

#endif
