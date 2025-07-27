#ifndef _GIZMO_GRID_H_
#define _GIZMO_GRID_H_

#include "../mesh/mesh.h"
#include "../camera/camera.h"
#include "../viewport/viewport.h"
#include <webgpu/webgpu.h>
#include "../utils/color.h"

typedef struct {
  color color;
  float size;
  float cell_size;
  float thickness;
} GizmoGridUniform;

typedef struct {

  GizmoGridUniform uniform;
  WGPUDevice *device;
  WGPUQueue *queue;

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
