#ifndef _GIZMO_GRID_H_
#define _GIZMO_GRID_H_

#include "../../../runtime/mesh.h"
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

} GridCreateDescriptor;

typedef struct {

  GridUniform uniform;

  WGPUDevice *device;
  WGPUQueue *queue;

  Camera *camera;
  Viewport *viewport;

} Grid;

void prefab_grid_create(Mesh *, GridCreateDescriptor *);

#endif
