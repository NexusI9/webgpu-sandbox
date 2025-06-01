#include "solid.h"

void material_solid_bind_views(Mesh *mesh, Camera *camera, Viewport *viewport,
                               uint8_t group_index) {
  material_bind_views(mesh, mesh_shader_solid, camera, viewport, group_index);
}


