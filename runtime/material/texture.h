#ifndef _MATERIAL_TEXTURE_H_
#define _MATERIAL_TEXTURE_H_
#include "core.h"

// bind
void material_texture_bind_views(Mesh *, Camera *, Viewport *,
                                 shader_bindgroup_t);
void material_texture_bind_lights(Mesh *, LightList *, uint8_t);

void material_texture_bind_ambient_occlusion(Mesh *, WGPUTextureView);
void material_texture_bind_shadow_maps(Mesh *, WGPUTextureView,
                                       WGPUTextureView);

// update
void material_texture_update_ambient_occlusion(Mesh *, WGPUTextureView);
void material_texture_update_shadow_maps(Mesh *, WGPUTextureView,
                                         WGPUTextureView);

// add
void material_texture_add_uniform(Mesh *,
                                  const ShaderCreateUniformDescriptor *);

void material_texture_add_texture(Mesh *,
                                  const ShaderCreateTextureDescriptor *);

void material_texture_add_texture_view(
    Mesh *, const ShaderCreateTextureViewDescriptor *);

void material_texture_add_sampler(Mesh *,
                                  const ShaderCreateSamplerDescriptor *);

// pipelines
void material_texture_double_sided(Mesh *);

// clear
void material_texture_clear_bindings(Mesh *);

#endif
