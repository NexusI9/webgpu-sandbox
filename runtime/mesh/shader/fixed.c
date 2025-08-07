#include "fixed.h"
#include "./utils.h"

void mesh_shader_fixed_bind_views(Mesh *mesh, Camera *camera, Viewport *viewport) {
  mesh_shader_bind_views(mesh, mesh_shader_fixed, camera, viewport);
}


/**
   Transfer Uniform to the right mesh shader (texture)
 */
void mesh_shader_fixed_add_uniform(Mesh *mesh,
                                  const ShaderCreateUniformDescriptor *desc) {
  shader_add_uniform(mesh_shader_fixed(mesh), desc);
}

/**
   Transfer Texture to the right mesh shader (texture)
 */
void mesh_shader_fixed_add_texture(Mesh *mesh,
                                  const ShaderCreateTextureDescriptor *desc) {
  shader_add_texture(mesh_shader_fixed(mesh), desc);
}

/**
   Transfer Texture View to the right mesh shader (texture)
 */
void mesh_shader_fixed_add_texture_view(
    Mesh *mesh, const ShaderCreateTextureViewDescriptor *desc) {
  shader_add_texture_view(mesh_shader_fixed(mesh), desc);
}

/**
   Transfer Sampler to the right mesh shader (texture)
 */
void mesh_shader_fixed_add_sampler(Mesh *mesh,
                                  const ShaderCreateSamplerDescriptor *desc) {
  shader_add_sampler(mesh_shader_fixed(mesh), desc);
}
