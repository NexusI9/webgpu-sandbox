#ifndef _GIZMO_BILLBOARD_H_
#define _GIZMO_BILLBOARD_H_

#include <cglm/cglm.h>
#include <stddef.h>
#include <webgpu/webgpu.h>

#include "../mesh/mesh.h"

typedef struct {
  vec3 *position;
  vec3 *scale;
  WGPUDevice *device;
  WGPUQueue *queue;
  const char *label;
  const char *texture_path;
  Camera *camera;
  Viewport *viewport;
} GizmoCreateBillboardDescriptor;


void gizmo_create_billboard(Mesh *, const GizmoCreateBillboardDescriptor *);

#endif
