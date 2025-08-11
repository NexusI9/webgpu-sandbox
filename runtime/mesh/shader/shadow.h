#ifndef _MESH_SHADER_SHADOW_H_
#define _MESH_SHADER_SHADOW_H_
#include "../runtime/mesh/core.h"
#include <webgpu/webgpu.h>

typedef struct {
  WGPUTextureView *texture_view;
  uint8_t sampler_binding;
  uint8_t texture_binding;
} MaterialShadowBindMapsDescriptor;

void mesh_shader_shadow_init_shader(Mesh *);
void mesh_shader_shadow_clear_bindings(Mesh *);

// bind shadow specicif view
void mesh_shader_shadow_update_mvp(Mesh *);

void mesh_shader_shadow_update_view(Mesh *, mat4 *);

void mesh_shader_shadow_update_model(Mesh *);

void mesh_shader_shadow_set_cullmode(Mesh *, const WGPUCullMode);

void mesh_shader_shadow_update_cullmode(Mesh *, const WGPUCullMode);

#endif
