#include "wireframe.h"

void mesh_shader_wireframe_update_mvp(Mesh *mesh, Camera *camera,
                                      Viewport *viewport) {
  mesh_shader_update_mvp(mesh, MeshShader_Wireframe, camera, viewport);
}
