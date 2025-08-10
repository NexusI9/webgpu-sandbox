#include "wireframe.h"

void mesh_shader_wireframe_update_mvp(Mesh *mesh, Camera *camera,
                                   Viewport *viewport) {
  mesh_shader_update_mvp(mesh, mesh_shader_wireframe, camera, viewport);
}
