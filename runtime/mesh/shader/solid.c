#include "solid.h"
#include "./utils.h"

void mesh_shader_solid_bind_views(Mesh *mesh, Camera *camera, Viewport *viewport) {
  mesh_shader_bind_views_any(mesh, mesh_shader_solid, camera, viewport);
}
