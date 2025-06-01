#ifndef _MATERIAL_SHADOW_H_
#define _MATERIAL_SHADOW_H_
#include "../mesh/mesh.h"
#include <webgpu/webgpu.h>

typedef struct {
  WGPUTextureView *texture_view;
  uint8_t sampler_binding;
  uint8_t texture_binding;
} MaterialShadowBindMapsDescriptor;

void material_shadow_init_shader(Mesh *);
void material_shadow_clear_bindings(Mesh *);

// bind shadow specicif view
void material_shadow_bind_views(Mesh *, mat4 *);

void material_shadow_set_cullmode(Mesh *, const WGPUCullMode);

#endif
