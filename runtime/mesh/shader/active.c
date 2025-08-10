#include "active.h"
#include "./utils.h"


void mesh_shader_active_update_views(Mesh *mesh, Camera *camera, Viewport *viewport) {
  mesh_shader_bind_views_any(mesh, mesh_shader_active, camera, viewport);
}

