#include "active.h"


void mesh_shader_active_update_mvp(Mesh *mesh, Camera *camera, Viewport *viewport) {
  mesh_shader_update_mvp(mesh, mesh_shader_active, camera, viewport);
}

