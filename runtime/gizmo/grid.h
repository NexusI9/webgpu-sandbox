#ifndef _GIZMO_GRID_H_
#define _GIZMO_GRID_H_

#include "../mesh.h"
#include <webgpu/webgpu.h>

typedef struct {
  vec4 color;
  float size;
  float cell_size;
  float thickness;
} GridUniform;

typedef struct {

  GridUniform uniform;

  WGPUDevice *device;
  WGPUQueue *queue;

  Camera *camera;
  Viewport *viewport;

} GizmoGridCreateDescriptor;

typedef struct {

  GridUniform uniform;

  WGPUDevice *device;
  WGPUQueue *queue;

  Camera *camera;
  Viewport *viewport;

} Grid;

void gizmo_grid_create(Mesh *, GizmoGridCreateDescriptor *);

#endif
