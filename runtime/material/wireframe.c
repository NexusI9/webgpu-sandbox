#include "wireframe.h"

void material_wireframe_bind_views(Mesh *mesh, Camera *camera,
                                   Viewport *viewport, uint8_t group_index) {
  material_bind_views(mesh, mesh_shader_wireframe, camera, viewport,
                      group_index);
}

