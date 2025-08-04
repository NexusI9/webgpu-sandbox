#ifndef _SCENE_EDITOR_OBJECT_BUILDER_BILLBOARD_H_
#define _SCENE_EDITOR_OBJECT_BUILDER_BILLBOARD_H_

#define GIZMO_BILLBOARD_SCALE ((vec3){0.85f, 0.85f, 0.85f})

#include <cglm/cglm.h>
#include <stddef.h>
#include <webgpu/webgpu.h>

#include "../runtime/mesh/mesh.h"

typedef struct {
  vec3 *position;
  vec3 *scale;
  WGPUDevice *device;
  WGPUQueue *queue;
  const char *label;
  const char *texture_path;
} GizmoCreateBillboardDescriptor;


void gizmo_create_billboard(Mesh *, const GizmoCreateBillboardDescriptor *);

#endif
