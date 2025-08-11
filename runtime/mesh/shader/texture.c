#include "texture.h"
#include "../backend/renderer/scene/scene.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

void mesh_shader_texture_update_mvp(Mesh *mesh, Camera *camera,
                                    Viewport *viewport) {
  mesh_shader_update_mvp(mesh, mesh_shader_texture, camera, viewport);
}

/**
   Clear the texture shader bind groups of mesh
 */
void mesh_shader_texture_clear_bindings(Mesh *mesh) {
  shader_bind_group_clear(mesh_shader_texture(mesh));
}

/**
   Init light list uniforms
   due to WGSL array uniforms necessity to have constant size, uniforms lists
   are already set at <light_list>[12], init them all to 0
   by default we will upload all the lights (point, ambient, spot)
   within a defined group

   TODO OPTI: currently we update all the lights on each update, implement a more
   targetted way to update lights based on their index.
  */
void mesh_shader_texture_update_lights(Mesh *mesh, LightList *light_list,
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
          /*.update =
                {
                     .callback = point_light_list_update_callback,
                     .trigger = point_light_list_trigger_callback,
                     .data = point_list,
                 },*/
      },
      // sun light
      {
          .binding = 3,
          .data = &sun_uniform,
          .offset = 0,
          .size = sizeof(SunLightListUniform),
      },
  };

  for (size_t i = 0; i < 4; i++) {
    ShaderBindGroupUniformEntry *entry = &entries[i];
    shader_update_uniform(mesh_shader_texture(mesh), group_index,
                          entry->binding, entry->data);
  }
}

/**
   Bind the ambient occlusion maps and sampler to the default shader (called
   during shader creation)
 */
void mesh_shader_texture_bind_ambient_occlusion(
    Mesh *mesh, WGPUTextureView ao_texture_view) {

  shader_update_texture_view(mesh_shader_texture(mesh), 0, 8, ao_texture_view,
                             AO_TEXTURE_FORMAT);

  shader_update_sampler(mesh_shader_texture(mesh), 0, 9,
                        &(WGPUSamplerDescriptor){
                            .addressModeU = WGPUAddressMode_ClampToEdge,
                            .addressModeV = WGPUAddressMode_ClampToEdge,
                            .addressModeW = WGPUAddressMode_ClampToEdge,
                            .minFilter = WGPUFilterMode_Linear,
                            .magFilter = WGPUFilterMode_Linear,
                            .compare = WGPUCompareFunction_Undefined,
                        });
}

/**
   Bind the shadow maps and sampler to the default shader (called during shader
   creation)
 */
void mesh_shader_texture_bind_shadow_maps(Mesh *mesh,
                                          WGPUTextureView point_texture_view,
                                          WGPUTextureView spot_texture_view) {

  const uint8_t sampler_binding = 5;
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
  shader_update_texture_view(mesh_shader_texture(mesh), group_index,
                             SHADER_TEXTURE_BINDING_POINT_TEXTURE_MAP,
                             point_texture_view, texture_format);

  shader_update_texture_view(mesh_shader_texture(mesh), group_index,
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

  shader_update_sampler(mesh_shader_texture(mesh), group_index, sampler_binding,
                        &sampler);

  shader_update_sampler(mesh_shader_texture(mesh), group_index,
                        sampler_binding + 2, &sampler);
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
void mesh_shader_texture_update_ambient_occlusion(Mesh *mesh,
                                                  WGPUTextureView map) {

  VERBOSE_PROCESS("Update AO map: %s", mesh->name);
  Shader *shader = mesh_shader_texture(mesh);
  shader_update_texture_view(shader, SHADER_TEXTURE_BINDGROUP_TEXTURES,
                             SHADER_TEXTURE_BINDING_AO, map, AO_TEXTURE_FORMAT);
}

void mesh_shader_texture_update_shadow_maps(Mesh *mesh,
                                            WGPUTextureView point_map,
                                            WGPUTextureView spot_map) {

  VERBOSE_PROCESS("Update shadow map: %s", mesh->name);
  Shader *shader = mesh_shader_texture(mesh);

  // update textures
  shader_update_texture_view(shader, SHADER_TEXTURE_BINDGROUP_LIGHTS,
                             SHADER_TEXTURE_BINDING_POINT_TEXTURE_MAP,
                             point_map, SHADOW_DEPTH_FORMAT);

  shader_update_texture_view(shader, SHADER_TEXTURE_BINDGROUP_LIGHTS,
                             SHADER_TEXTURE_BINDING_DIR_TEXTURE_MAP, spot_map,
                             SHADOW_DEPTH_FORMAT);
}

/**
  update pipeline for double-sided
 */
void mesh_shader_texture_double_sided(Mesh *mesh) {

  /* STDPIPELINE TEXTURE
  pipeline_set_primitive(shader_pipeline(mesh_shader_texture(mesh)),
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_None,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });

  */
}
