#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include "light.h"
#include "mesh.h"
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
void material_init_shader_texture(mesh *);
void material_init_shader_shadow(mesh *);

void material_clear_bindings(mesh *, MeshDrawMethod);

// === TEXTURE SHADER ===

// bind model, camera and viewport to bind group
void material_texture_bind_views(mesh *, camera *, viewport *, uint8_t);
// bind light scene
void material_texture_bind_lights(mesh *, viewport *, AmbientLightList *,
                                  SpotLightList *, PointLightList *,
                                  SunLightList *, uint8_t);

void material_texture_add_uniform(mesh *,
                                  const ShaderCreateUniformDescriptor *);

void material_texture_add_texture(mesh *,
                                  const ShaderCreateTextureDescriptor *);

void material_texture_add_texture_view(
    mesh *, const ShaderCreateTextureViewDescriptor *);

void material_texture_add_sampler(mesh *,
                                  const ShaderCreateSamplerDescriptor *);
void material_texure_bind_shadow_maps(mesh *, WGPUTextureView, WGPUTextureView);

// === SHADOW SHADER ===

typedef struct {
  WGPUTextureView *texture_view;
  uint8_t sampler_binding;
  uint8_t texture_binding;
} MaterialShadowBindMapsDescriptor;

// bind shadow specicif view
void material_shadow_bind_views(mesh *, mat4 *);

void material_shadow_set_cullmode(mesh *, const WGPUCullMode);

#endif
