#include "fixed.h"
#include "core.h"


void mesh_shader_fixed_update_mvp(Mesh *mesh, Camera *camera,
                                  Viewport *viewport) {
  mesh_shader_update_mvp(mesh, mesh_shader_fixed, camera, viewport);
}
