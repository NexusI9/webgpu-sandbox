#ifndef _SCENE_EDITOR_OBJECT_GRID_H_
#define _SCENE_EDITOR_OBJECT_GRID_H_

#include <webgpu/webgpu.h>

#include "runtime/camera/camera.h"
#include "runtime/mesh/mesh.h"
#include "runtime/viewport/viewport.h"
#include "utils/color.h"
#include "runtime/camera/core.h"
#include "runtime/mesh/core.h"
#include "runtime/viewport/core.h"

typedef struct {
  color color;
  float size;
  float cell_size;
  float thickness;
} GizmoGridUniform;

typedef struct {

  GizmoGridUniform uniform;

} GizmoGridCreateDescriptor;

typedef struct {

  GizmoGridUniform uniform;

  Camera *camera;
  Viewport *viewport;

} GizmoGrid;

void seo_grid_create(Mesh *, GizmoGridCreateDescriptor *);

#endif
