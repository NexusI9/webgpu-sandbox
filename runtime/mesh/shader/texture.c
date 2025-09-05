#include "texture.h"
#include "../backend/renderer/scene/scene.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

/**
   Clear the texture shader bind groups of mesh
 */
void mesh_shader_texture_clear_bindings(Mesh *mesh) {
  shader_bind_group_clear(mesh_shader(mesh, MeshShader_Texture));
}

/**
   Init light list uniforms
   due to WGSL array uniforms necessity to have constant size, uniforms lists
   are already set at <light_list>[12], init them all to 0
   by default we will upload all the lights (point, ambient, spot)
   within a defined group

   TODO OPTI: currently we update all the lights on each update, implement a
   more targetted way to update lights based on their index.
  */
void mesh_shader_texture_update_lights(Mesh *mesh, const MeshShader shader_type,
                               UBOManager *ubo, SSBOManager *ssbo) {

  WGPUBuffer entries[5] = {
      ssbo_buffer_handle(ssbo, SSBOType_AmbientLight),
      ssbo_buffer_handle(ssbo, SSBOType_SpotLight),
      ssbo_buffer_handle(ssbo, SSBOType_PointLight),
      ssbo_buffer_handle(ssbo, SSBOType_SunLight),
      ubo_buffer_handle(ubo),
  };

  for (size_t i = 0; i < 5; i++)
    shader_update_uniform_buffer(mesh_shader(mesh, shader_type),
                                 SHADER_TEXTURE_BINDGROUP_LIGHTS, i, entries[i],
                                 0, ShaderBufferLifetime_Release);
}

/**
   Bind the shadow maps and sampler to the default shader (called during shader
   creation)
 */
void mesh_shader_texture_bind_shadow_maps(Mesh *mesh,
                                          WGPUTextureView point_texture_view,
                                          WGPUTextureView spot_texture_view) {

  const uint8_t sampler_binding = 6;
  const uint8_t group_index = 2;

  // create texture views

#ifdef RENDER_SHADOW_AS_COLOR
  const WGPUTextureFormat texture_format = SHADOW_COLOR_FORMAT;
  const WGPUTextureSampleType texture_sample_type = WGPUTextureSampleType_Float;
  const WGPUSamplerBindingType sample_type = WGPUSamplerBindingType_Filtering;
  const WGPUCompareFunction sample_compare = WGPUCompareFunction_Undefined;
#else
  const WGPUTextureFormat texture_format = SHADOW_DEPTH_FORMAT;
  const WGPUTextureSampleType texture_sample_type = WGPUTextureSampleType_Depth;
  const WGPUSamplerBindingType sample_type = WGPUSamplerBindingType_Comparison;
  const WGPUCompareFunction sample_compare = WGPUCompareFunction_Less;
#endif

  // add multi-layered texture to default shader
  shader_update_texture_view(mesh_shader(mesh, MeshShader_Texture), group_index,
                             SHADER_TEXTURE_BINDING_POINT_TEXTURE_MAP,
                             point_texture_view, texture_format);

  shader_update_texture_view(mesh_shader(mesh, MeshShader_Texture), group_index,
                             SHADER_TEXTURE_BINDING_DIR_TEXTURE_MAP,
                             spot_texture_view, texture_format);

  // add related sampler to default shader
  // NOTE: With depth texture need to use a special sampler type:
  // Comparison

  WGPUSamplerDescriptor sampler = {
      .addressModeU = WGPUAddressMode_ClampToEdge,
      .addressModeV = WGPUAddressMode_ClampToEdge,
      .addressModeW = WGPUAddressMode_ClampToEdge,
      .magFilter = WGPUFilterMode_Nearest,
      .minFilter = WGPUFilterMode_Nearest,
      .compare = sample_compare,
  };

  const MeshShader shader_types[2] = {
      MeshShader_Texture,
      MeshShader_Reflection,
  };

  for (uint8_t i = 0; i < 2; i++) {

    Shader *shader = mesh_shader(mesh, shader_types[i]);

    shader_update_sampler(shader, group_index, sampler_binding, &sampler);

    shader_update_sampler(shader, group_index, sampler_binding + 2, &sampler);
  }
}

void mesh_shader_texture_update_shadow_maps(Mesh *mesh,
                                            WGPUTextureView point_map,
                                            WGPUTextureView spot_map) {

  VERBOSE_PROCESS("Update shadow map: %s", mesh->name);

  const MeshShader shader_types[2] = {
      MeshShader_Texture,
      MeshShader_Reflection,
  };

  for (uint8_t i = 0; i < 2; i++) {

    Shader *shader = mesh_shader(mesh, shader_types[i]);

    // update point texture
    shader_update_texture_view(shader, SHADER_TEXTURE_BINDGROUP_LIGHTS,
                               SHADER_TEXTURE_BINDING_POINT_TEXTURE_MAP,
                               point_map, SHADOW_DEPTH_FORMAT);

    // update dir texture
    shader_update_texture_view(shader, SHADER_TEXTURE_BINDGROUP_LIGHTS,
                               SHADER_TEXTURE_BINDING_DIR_TEXTURE_MAP, spot_map,
                               SHADOW_DEPTH_FORMAT);
  }
}

/**
  update pipeline for double-sided
 */
void mesh_shader_texture_double_sided(Mesh *mesh) {

  /* STDPIPELINE TEXTURE
  pipeline_set_primitive(shader_pipeline(mesh_shader(mesh, MeshShader_Texture)),
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_None,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });

  */
}
