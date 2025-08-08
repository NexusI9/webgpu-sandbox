#include "wireframe.h"
#include "./utils.h"

void mesh_shader_wireframe_bind_views(Mesh *mesh, Camera *camera,
                                   Viewport *viewport) {
  mesh_shader_bind_views_any(mesh, mesh_shader_wireframe, camera, viewport);
}
