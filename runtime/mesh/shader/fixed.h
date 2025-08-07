#ifndef _MESH_SHADER_FIXED_H_
#define _MESH_SHADER_FIXED_H_
#include "../runtime/camera/camera.h"
#include "core.h"

void mesh_shader_fixed_bind_views(Mesh *, Camera *, Viewport *);

void mesh_shader_fixed_add_uniform(Mesh *,
                                     const ShaderCreateUniformDescriptor *);

void mesh_shader_fixed_add_texture(Mesh *,
                                     const ShaderCreateTextureDescriptor *);

void mesh_shader_fixed_add_texture_view(
    Mesh *, const ShaderCreateTextureViewDescriptor *);

void mesh_shader_fixed_add_sampler(Mesh *,
                                     const ShaderCreateSamplerDescriptor *);

#endif
