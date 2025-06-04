#ifndef _GIZMO_WIREFRAME_H_
#define _GIZMO_WIREFRAME_H_
#include "core.h"

#define GIZMO_WIREFRAME_LINE_THICKNESS 0.005f

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  VertexAttribute *vertex;
  VertexIndex *index;
  vec3 *color;
  float thickness;
  const char *name;

} GizmoCreateWireframeDescriptor;

void gizmo_create_wireframe(Mesh *, const GizmoCreateWireframeDescriptor *);

#endif
