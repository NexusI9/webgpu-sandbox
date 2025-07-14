#ifndef _GIZMO_GRID_H_
#define _GIZMO_GRID_H_

#include "../mesh/mesh.h"
#include "../camera/camera.h"
#include "../viewport/viewport.h"
#include <webgpu/webgpu.h>

typedef struct {
  vec4 color;
  float size;
  float cell_size;
  float thickness;
} GizmoGridUniform;

typedef struct {

  GizmoGridUniform uniform;

  WGPUDevice *device;
  WGPUQueue *queue;

  Camera *camera;
  Viewport *viewport;

} GizmoGridCreateDescriptor;

typedef struct {

  GizmoGridUniform uniform;

  WGPUDevice *device;
  WGPUQueue *queue;

  Camera *camera;
  Viewport *viewport;

} GizmoGrid;

void gizmo_grid_create(Mesh *, GizmoGridCreateDescriptor *);

#endif
