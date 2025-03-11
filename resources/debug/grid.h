#ifndef _GRID_H_
#define _GRID_H_

#include "../../runtime/mesh.h"
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

  camera *camera;
  viewport *viewport;

} GridCreateDescriptor;

typedef struct {

  GridUniform uniform;

  WGPUDevice *device;
  WGPUQueue *queue;

  camera *camera;
  viewport *viewport;

} grid;

void grid_create_mesh(mesh *, GridCreateDescriptor *);

#endif
