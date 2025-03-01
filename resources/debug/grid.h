#ifndef _GRID_H_
#define _GRID_H_

#include "../../runtime/mesh.h"
#include <webgpu/webgpu.h>

typedef struct {
  float size;
  float cell_size;
} GridUniform;

typedef struct {

  uint16_t size;
  float cell_size;

  WGPUDevice *device;
  WGPUQueue *queue;

  camera *camera;
  viewport *viewport;

} GridCreateDescriptor;

typedef struct {

  float size;
  float cell_size;

  WGPUDevice *device;
  WGPUQueue *queue;

  camera *camera;
  viewport *viewport;

} grid;

GridUniform grid_uniform(const grid *);
mesh grid_create_mesh(const GridCreateDescriptor *);

#endif
