#ifndef _MATERIAL_TEXTURE_H_
#define _MATERIAL_TEXTURE_H_
#include "core.h"

void material_texture_init_shader(Mesh *);
void material_texture_clear_bindings(Mesh *);

// bind model, camera and viewport to bind group
void material_texture_bind_views(Mesh *, Camera *, Viewport *,
                                 shader_bindgroup_t);
// bind light scene
void material_texture_bind_lights(Mesh *, LightList *, uint8_t);

void material_texture_add_uniform(Mesh *,
                                  const ShaderCreateUniformDescriptor *);

void material_texture_add_texture(Mesh *,
                                  const ShaderCreateTextureDescriptor *);

void material_texture_double_sided(Mesh *);

void material_texture_add_texture_view(
    Mesh *, const ShaderCreateTextureViewDescriptor *);

void material_texture_add_sampler(Mesh *,
                                  const ShaderCreateSamplerDescriptor *);
void material_texure_bind_shadow_maps(Mesh *, WGPUTextureView, WGPUTextureView);

#endif
