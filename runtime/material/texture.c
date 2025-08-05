#include "texture.h"
#include "../../backend/renderer/renderer.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

/**
   Clear the texture shader bind groups of mesh
 */
void material_texture_clear_bindings(Mesh *mesh) {
  shader_bind_group_clear(mesh_shader_texture(mesh));
}

void material_texture_bind_views(Mesh *mesh, Camera *camera, Viewport *viewport,
                                 uint8_t group_index) {
  material_bind_views(mesh, mesh_shader_texture, camera, viewport, group_index);
}

/**
   Init light list uniforms
   due to WGSL array uniforms necessity to have constant size, uniforms lists
   are already set at <light_list>[12], init them all to 0
   by default we will upload all the lights (point, ambient, spot)
   within a defined group
  */
void material_texture_bind_lights(Mesh *mesh, LightList *light_list,
                                  uint8_t group_index) {

  AmbientLightList *ambient_list = &light_list->ambient;
  SpotLightList *spot_list = &light_list->spot;
  SunLightList *sun_list = &light_list->sun;
  PointLightList *point_list = &light_list->point;

  AmbientLightListUniform ambient_uniform;
  SpotLightListUniform spot_uniform;
  PointLightListUniform point_uniform;
  SunLightListUniform sun_uniform;

  if (ambient_list) {
    // update length
    ambient_uniform.length = ambient_list->length;
    // update entries
    for (size_t i = 0; i < ambient_uniform.length; i++) {
      AmbientLight *light = &ambient_list->entries[i];
      AmbientLightUniform *uniform = &ambient_uniform.entries[i];
      ambient_light_uniform(uniform, light);
    }
  }

  if (spot_list) {
    // update length
    spot_uniform.length = spot_list->length;
    // update entries
    for (size_t i = 0; i < spot_uniform.length; i++) {
      SpotLight *light = &spot_list->entries[i];
      SpotLightUniform *uniform = &spot_uniform.entries[i];
      spot_light_uniform(uniform, light);
    }
  }

  if (point_list) {
    // update length
    point_uniform.length = point_list->length;
    // update entries
    for (size_t i = 0; i < point_uniform.length; i++) {
      PointLight *light = &point_list->entries[i];
      PointLightUniform *uniform = &point_uniform.entries[i];
      point_light_uniform(uniform, light);
    }
  }

  if (sun_list) {

    sun_uniform.length = sun_list->length;

    for (size_t i = 0; i < sun_uniform.length; i++) {
      SunLight *light = &sun_list->entries[i];
      SunLightUniform *uniform = &sun_uniform.entries[i];
      sun_light_uniform(uniform, light);
    }
  }

  ShaderBindGroupUniformEntry entries[4] = {
      // ambient light
      {
          .binding = 0,
          .data = &ambient_uniform,
          .offset = 0,
          .size = sizeof(AmbientLightListUniform),

      },
      // spot light
      {
          .binding = 1,
          .data = &spot_uniform,
          .offset = 0,
          .size = sizeof(SpotLightListUniform),
      },
      // point light
      {
          .binding = 2,
          .data = &point_uniform,
          .offset = 0,
          .size = sizeof(PointLightListUniform),
          .update =
              {
                  .callback = point_light_list_update_callback,
                  .trigger = point_light_list_trigger_callback,
                  .data = point_list,
              },
      },
      // sun light
      {
          .binding = 3,
          .data = &sun_uniform,
          .offset = 0,
          .size = sizeof(SunLightListUniform),
      },
  };

  shader_add_uniform(
      &mesh->shader.texture,
      &(ShaderCreateUniformDescriptor){
          .group_index = group_index,
          .entry_count = 4,
          .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
          .entries = entries,
      });
}

/**
   Bind the ambient occlusion maps and sampler to the default shader (called
   during shader creation)
 */
void material_texture_bind_ambient_occlusion(Mesh *mesh,
                                             WGPUTextureView ao_texture_view) {

  shader_add_texture_view(
      mesh_shader_texture(mesh),
      &(ShaderCreateTextureViewDescriptor){
          .group_index = 0,
          .entry_count = 1,
          .entries = (ShaderBindGroupTextureViewEntry[]){
              {
                  .binding = 8,
                  .texture_view = ao_texture_view,
                  .dimension = WGPUTextureViewDimension_2D,
                  .format = AO_TEXTURE_FORMAT,
                  .sample_type = WGPUTextureSampleType_Float,
              },
          }});

  shader_add_sampler(mesh_shader_texture(mesh),
                     &(ShaderCreateSamplerDescriptor){
                         .group_index = 0,
                         .entry_count = 1,
                         .entries = (ShaderBindGroupSamplerEntry[]){
                             {
                                 .binding = 9,
                                 .type = WGPUSamplerBindingType_Filtering,
                                 .addressModeU = WGPUAddressMode_ClampToEdge,
                                 .addressModeV = WGPUAddressMode_ClampToEdge,
                                 .addressModeW = WGPUAddressMode_ClampToEdge,
                                 .minFilter = WGPUFilterMode_Linear,
                                 .magFilter = WGPUFilterMode_Linear,
                                 .compare = WGPUCompareFunction_Undefined,
                             },
                         }});
}

/**
   Bind the shadow maps and sampler to the default shader (called during shader
   creation)
 */
void material_texture_bind_shadow_maps(
    Mesh *mesh, WGPUTextureView fallback_point_texture_view,
    WGPUTextureView fallback_spot_texture_view) {

  const uint8_t sampler_binding = 5;
  const uint8_t group_index = 2;

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
  shader_add_texture_view(
      mesh_shader_texture(mesh),
      &(ShaderCreateTextureViewDescriptor){
          .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
          .entry_count = 2,
          .group_index = group_index,
          .entries =
              (ShaderBindGroupTextureViewEntry[]){
                  {
                      .binding = SHADER_TEXTURE_BINDING_POINT_TEXTURE_MAP,
                      .texture_view = fallback_point_texture_view,
                      .dimension = WGPUTextureViewDimension_CubeArray,
                      .format = texture_format,
                      .sample_type = texture_sample_type,
                  },
                  {
                      .binding = SHADER_TEXTURE_BINDING_DIR_TEXTURE_MAP,
                      .texture_view = fallback_spot_texture_view,
                      .dimension = WGPUTextureViewDimension_2DArray,
                      .format = texture_format,
                      .sample_type = texture_sample_type,
                  },
              },
      });

  // add related sampler to default shader
  // NOTE: With depth texture need to use a special sampler type:
  // Comparison

  ShaderBindGroupSamplerEntry point_sampler = {
      .binding = sampler_binding,
      .type = sample_type,
      .addressModeU = WGPUAddressMode_ClampToEdge,
      .addressModeV = WGPUAddressMode_ClampToEdge,
      .addressModeW = WGPUAddressMode_ClampToEdge,
      .magFilter = WGPUFilterMode_Nearest,
      .minFilter = WGPUFilterMode_Nearest,
      .compare = sample_compare,
  };

  ShaderBindGroupSamplerEntry spot_sampler = point_sampler;
  spot_sampler.binding += 2;

  shader_add_sampler(
      mesh_shader_texture(mesh),
      &(ShaderCreateSamplerDescriptor){
          .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
          .entry_count = 2,
          .group_index = group_index,
          .entries =
              (ShaderBindGroupSamplerEntry[]){
                  point_sampler,
                  spot_sampler,
              },
      });
}

/**
   Update the Ambient Occlusion texture view map of the given mesh.
   While the bind functions create and bind samplers + textures,
   the Update functions simply replace the Texture view.

   Replacing the texture view doesn't require to clear the whole pipeline, since
   the pipeline only cares about:
   - shader code
   - bing group layout (not their content)
   - vertex buffer layouts, formats etc.

   However we do need to rebuild the shader->bind_groups, since they are used
   during the draw loop.

   So the overall process is:

    Build shader  => compute maps => replace bind group => build shader
    (full layout)                                        (bind group only)

 */
void material_texture_update_ambient_occlusion(Mesh *mesh,
                                               WGPUTextureView map) {

  VERBOSE_PROCESS("Update AO map: %s", mesh->name);
  Shader *shader = mesh_shader_texture(mesh);
  shader_update_texture(shader, SHADER_TEXTURE_BINDGROUP_TEXTURES, &map,
                        SHADER_TEXTURE_BINDING_AO);
}

void material_texture_update_shadow_maps(Mesh *mesh, WGPUTextureView point_map,
                                         WGPUTextureView spot_map) {

  VERBOSE_PROCESS("Update shadow map: %s", mesh->name);
  Shader *shader = mesh_shader_texture(mesh);

  // update textures
  shader_update_texture(shader, SHADER_TEXTURE_BINDGROUP_LIGHTS, &point_map,
                        SHADER_TEXTURE_BINDING_POINT_TEXTURE_MAP);

  shader_update_texture(shader, SHADER_TEXTURE_BINDGROUP_LIGHTS, &spot_map,
                        SHADER_TEXTURE_BINDING_DIR_TEXTURE_MAP);
}

/**
   Transfer Uniform to the right mesh shader (texture)
 */
void material_texture_add_uniform(Mesh *mesh,
                                  const ShaderCreateUniformDescriptor *desc) {
  shader_add_uniform(mesh_shader_texture(mesh), desc);
}

/**
   Transfer Texture to the right mesh shader (texture)
 */
void material_texture_add_texture(Mesh *mesh,
                                  const ShaderCreateTextureDescriptor *desc) {
  shader_add_texture(mesh_shader_texture(mesh), desc);
}

/**
   Transfer Texture View to the right mesh shader (texture)
 */
void material_texture_add_texture_view(
    Mesh *mesh, const ShaderCreateTextureViewDescriptor *desc) {
  shader_add_texture_view(mesh_shader_texture(mesh), desc);
}

/**
   Transfer Sampler to the right mesh shader (texture)
 */
void material_texture_add_sampler(Mesh *mesh,
                                  const ShaderCreateSamplerDescriptor *desc) {
  shader_add_sampler(mesh_shader_texture(mesh), desc);
}

/**
  update pipeline for double-sided
 */
void material_texture_double_sided(Mesh *mesh) {
  pipeline_set_primitive(shader_pipeline(mesh_shader_texture(mesh)),
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_None,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });
}
