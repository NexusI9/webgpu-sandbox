#ifndef _MESH_SHADER_TEXTURE_H_
#define _MESH_SHADER_TEXTURE_H_
#include "core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/light/light.h"

// bind
void mesh_shader_texture_bind_views(Mesh *, Camera *, Viewport *);
void mesh_shader_texture_bind_lights(Mesh *, LightList *, uint8_t);

void mesh_shader_texture_bind_ambient_occlusion(Mesh *, WGPUTextureView);
void mesh_shader_texture_bind_shadow_maps(Mesh *, WGPUTextureView,
                                       WGPUTextureView);

// update
void mesh_shader_texture_update_ambient_occlusion(Mesh *, WGPUTextureView);
void mesh_shader_texture_update_shadow_maps(Mesh *, WGPUTextureView,
                                         WGPUTextureView);

// add
void mesh_shader_texture_add_uniform(Mesh *,
                                  const ShaderCreateUniformDescriptor *);

void mesh_shader_texture_add_texture(Mesh *,
                                  const ShaderCreateTextureDescriptor *);

void mesh_shader_texture_add_texture_view(
    Mesh *, const ShaderCreateTextureViewDescriptor *);

void mesh_shader_texture_add_sampler(Mesh *,
                                  const ShaderCreateSamplerDescriptor *);

// pipelines
void mesh_shader_texture_double_sided(Mesh *);

// clear
void mesh_shader_texture_clear_bindings(Mesh *);

#endif
