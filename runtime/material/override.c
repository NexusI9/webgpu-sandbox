#include "override.h"

void material_override_bind_views(Mesh *mesh, Camera *camera,
                                   Viewport *viewport, uint8_t group_index) {
  material_bind_views(mesh, mesh_shader_override, camera, viewport,
                      group_index);
}

