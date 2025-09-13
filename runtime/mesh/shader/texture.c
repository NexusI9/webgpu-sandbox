#include "texture.h"
#include "../runtime/light/shadow_map/core.h"
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

  Shader *shader = mesh_shader(mesh, MeshShader_Texture);
  const PipelineBinding *bindings = &shader->pipeline->bindings;

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
  shader_update_texture_view(shader, bindings->light_list->group,
                             bindings->light_list->point_texture,
                             point_texture_view, texture_format);

  shader_update_texture_view(shader, bindings->light_list->group,
                             bindings->light_list->directional_texture,
                             spot_texture_view, texture_format);
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
    const PipelineBinding *bindings = &shader->pipeline->bindings;

    // update point texture
    if (bindings->light_list->point_texture != PIPELINE_BINDING_UNDEFINED)
      shader_update_texture_view(shader, bindings->light_list->group,
                                 bindings->light_list->point_texture, point_map,
                                 SHADOW_DEPTH_FORMAT);

    // update dir texture
    if (bindings->light_list->directional_texture != PIPELINE_BINDING_UNDEFINED)
      shader_update_texture_view(shader, bindings->light_list->group,
                                 bindings->light_list->directional_texture,
                                 spot_map, SHADOW_DEPTH_FORMAT);
  }
}

void mesh_shader_texture_update_probes(Mesh *mesh,
                                       WGPUTextureView plane_texture,
                                       WGPUTextureView grid_texture,
                                       SSBOManager *ssbo) {

  Shader *shader = mesh_shader(mesh, MeshShader_Texture);
  const PipelineBinding *bindings = &shader->pipeline->bindings;

  // update plane texture
  if (bindings->probe->reflection_plane_texture != PIPELINE_BINDING_UNDEFINED)
    shader_update_texture_view(shader, bindings->probe->group,
                               bindings->probe->reflection_plane_texture,
                               plane_texture, TEXTURE_FORMAT_OFFSCREEN_DEFAULT);

  // update grid texture
  if (bindings->probe->reflection_grid_texture != PIPELINE_BINDING_UNDEFINED)
    shader_update_texture_view(shader, bindings->probe->group,
                               bindings->probe->reflection_grid_texture,
                               grid_texture, TEXTURE_FORMAT_OFFSCREEN_DEFAULT);
}

void mesh_shader_texture_update_environment(Mesh *mesh,
                                            WGPUTextureView skybox_texture,
                                            SSBOManager *ssbo) {

  Shader *shader = mesh_shader(mesh, MeshShader_Texture);
  const PipelineBinding *bindings = &shader->pipeline->bindings;

  // update skybox texture
  if (bindings->probe->skybox_texture != PIPELINE_BINDING_UNDEFINED)
    shader_update_texture_view(shader, bindings->probe->group,
                               bindings->probe->skybox_texture, skybox_texture,
                               TEXTURE_FORMAT_OFFSCREEN_DEFAULT);
}

/**
   Link one mesh texture to a reflection probe.
 */
void mesh_shader_texture_bind_probe(Mesh *mesh,
                                    const ProbeReflectionPlane *plane,
                                    SSBOManager *ssbo) {

  Shader *shader = mesh_shader(mesh, MeshShader_Texture);
  const Pipeline *pipeline = shader_pipeline(shader);

  
  shader_update_uniform_buffer(
      shader, pipeline->bindings.probe->group,
      pipeline->bindings.probe->reflection_plane,
      ssbo_buffer_handle(ssbo, SSBOType_ProbePlaneReflection),
      plane->ssbo_slot[ProbeReflectionSSBOField_List].id,
      ShaderBufferLifetime_Release);

  {
    MeshUniform *uniform = mesh_uniform(mesh);
    uniform->probe_reflection_plane_count = 1;
    ssbo_update_queue_insert(ssbo, SSBOType_Mesh, mesh->ssbo_slot.id);
  }
}
