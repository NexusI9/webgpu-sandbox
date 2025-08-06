#ifndef _SCENE_EDITOR_OBJECT_GRID_H_
#define _SCENE_EDITOR_OBJECT_GRID_H_

#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"
#include "../utils/color.h"
#include <webgpu/webgpu.h>

typedef struct {
  color color;
  float size;
  float cell_size;
  float thickness;
} GizmoGridUniform;

typedef struct {

  GizmoGridUniform uniform;
  const WGPUDevice device;
  const WGPUQueue queue;
  const Pipeline* pipeline;
} GizmoGridCreateDescriptor;

typedef struct {

  GizmoGridUniform uniform;

  const WGPUDevice device;
  const WGPUQueue queue;

  Camera *camera;
  Viewport *viewport;

} GizmoGrid;

void seo_grid_create(Mesh *, GizmoGridCreateDescriptor *);

#endif
