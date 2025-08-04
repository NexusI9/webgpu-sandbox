#ifndef _SCENE_EDITOR_OBJECT_GRID_H_
#define _SCENE_EDITOR_OBJECT_GRID_H_

#include "../runtime/mesh/mesh.h"
#include "../runtime/camera/camera.h"
#include "../runtime/viewport/viewport.h"
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

void seo_grid_create(Mesh *, GizmoGridCreateDescriptor *);

#endif
