#include "shadow.h"
#include "../utils/system.h"
#include "./core.h"

/**
   Bind a specific point light view to the mesh's shadow shader
   The function is called during the scene shadow updating process
   As to provide to the shadow shader each lights views.

   Note that the view matrix shall be combination of the [projection view] *
   [light view] already multiplied together as there is currently no need to
   upload separate views in the shader.
 */

void mesh_shader_shadow_update_mvp(Mesh *mesh) {

  MeshUniform *uModel = mesh_uniform(mesh);

  // views uniforms (will be replaced during shadow pass)
  shader_update_uniform(mesh_shader(mesh, MeshShader_Shadow), 0, 0, (void *)0);

  // mesh model matrix
  shader_update_uniform(mesh_shader(mesh, MeshShader_Shadow), 0, 1, uModel);
}

void mesh_shader_shadow_update_view(Mesh *mesh, mat4 *view) {
  shader_update_uniform(mesh_shader(mesh, MeshShader_Shadow), 0, 0, view);
}

void mesh_shader_shadow_update_model(Mesh *mesh) {

  MeshUniform *uModel = mesh_uniform(mesh);
  // mesh model matrix
  shader_update_uniform(mesh_shader(mesh, MeshShader_Shadow), 0, 1, uModel);
}

/**
   Clear the shadow shader bind groups of mesh
 */
void mesh_shader_shadow_clear_bindings(Mesh *mesh) {
  shader_bind_group_clear(mesh_shader(mesh, MeshShader_Shadow));
}
