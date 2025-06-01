#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include "light.h"
#include "mesh/mesh.h"
#include "shader.h"
#include "webgpu/webgpu.h"

/**
   ============================= MATERIAL API ================================

   The material api serve as a bridge between the scene elements, the mesh and
   the shader.

   The material api functions are basically predefined binding layout
   for each standard shaders (texture/ shadow/ wireframe).

   Usually the functions will take a mesh as a first argument.
   Depending on the function, the right standard shader will be automatically
   modified/bound.

   This API serve a more polished and synthesized way to edit and initialize
   standard shaders. Preventing to manually adjust the shader and pipeline.

      .-----------.                                      .-----------------.
      | Scene     |---.      .--------------.      .---> | Texture Shader  |
      '-----------'   |      |     Mesh     |      |     '-----------------'
      .-----------.   |      '--------------'      |
      | Light     |---|             |              |
      '-----------'   |      .--------------.      |     .-----------------.
      .-----------.   +----> | Material API |------+---> | Shadow Shader   |
      | Camera    |---|      '--------------'      |     '-----------------'
      '-----------'   |                            |
      .-----------.   |                            |     .------------------.
      | Viewport  |---'                            '---> | Wireframe Shader |
      '-----------'                                      '------------------'


 */

// ====== COMMONS ======

// create related pipelines
void material_init_shader_texture(Mesh *);
void material_init_shader_shadow(Mesh *);

void material_clear_bindings_texture(Mesh *);
void material_clear_bindings_shadow(Mesh *);

// === TEXTURE SHADER ===

// bind model, camera and viewport to bind group
void material_texture_bind_views(Mesh *, Camera *, Viewport *,
                                 shader_bindgroup_t);
// bind light scene
void material_texture_bind_lights(Mesh *, AmbientLightList *, SpotLightList *,
                                  PointLightList *, SunLightList *, uint8_t);

void material_texture_add_uniform(Mesh *,
                                  const ShaderCreateUniformDescriptor *);

void material_texture_add_texture(Mesh *,
                                  const ShaderCreateTextureDescriptor *);

void material_texture_add_texture_view(
    Mesh *, const ShaderCreateTextureViewDescriptor *);

void material_texture_add_sampler(Mesh *,
                                  const ShaderCreateSamplerDescriptor *);
void material_texure_bind_shadow_maps(Mesh *, WGPUTextureView, WGPUTextureView);

// === WIREFRAME SHADER ===
void material_wireframe_bind_views(Mesh *, Camera *, Viewport *, uint8_t);

// === SOLID SHADER ===
void material_solid_bind_views(Mesh *, Camera *, Viewport *, uint8_t);

// === SHADOW SHADER ===

typedef struct {
  WGPUTextureView *texture_view;
  uint8_t sampler_binding;
  uint8_t texture_binding;
} MaterialShadowBindMapsDescriptor;

// bind shadow specicif view
void material_shadow_bind_views(Mesh *, mat4 *);

void material_shadow_set_cullmode(Mesh *, const WGPUCullMode);

#endif
