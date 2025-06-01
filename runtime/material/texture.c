#include "texture.h"
#include "../../backend/shadow_pass.h"


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
void material_texture_bind_lights(Mesh *mesh, AmbientLightList *ambient_list,
                                  SpotLightList *spot_list,
                                  PointLightList *point_list,
                                  SunLightList *sun_list, uint8_t group_index) {

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

      // map light to light uniform (including paddings...)
      *uniform = (AmbientLightUniform){0};
      uniform->intensity = light->intensity;
      glm_vec3_copy(light->color, uniform->color);
    }
  }

  if (spot_list) {
    // update length
    spot_uniform.length = spot_list->length;
    // update entries
    for (size_t i = 0; i < spot_uniform.length; i++) {
      SpotLight *light = &spot_list->entries[i];
      SpotLightUniform *uniform = &spot_uniform.entries[i];

      // TODO: create a light method "light_spot_uniform"
      *uniform = (SpotLightUniform){0};
      uniform->intensity = light->intensity;
      uniform->cutoff = light->cutoff;
      uniform->inner_cutoff = light->inner_cutoff;
      glm_vec3_copy(light->color, uniform->color);
      glm_vec3_copy(light->target, uniform->target);
      glm_vec3_copy(light->position, uniform->position);

      // get light view matrix
      LightViews spot_view =
          light_spot_view(light->position, light->target, light->angle);

      glm_mat4_copy(spot_view.views[0], uniform->view);
    }
  }

  if (point_list) {
    // update length
    point_uniform.length = point_list->length;
    // update entries
    for (size_t i = 0; i < point_uniform.length; i++) {
      PointLight *light = &point_list->entries[i];
      PointLightUniform *uniform = &point_uniform.entries[i];

      *uniform = (PointLightUniform){0};
      uniform->intensity = light->intensity;
      uniform->cutoff = light->cutoff;
      uniform->inner_cutoff = light->inner_cutoff;
      uniform->near = light->near;
      uniform->far = light->far;

      glm_vec3_copy(light->color, uniform->color);
      glm_vec3_copy(light->position, uniform->position);

      // copy 6 points views for shader depth comparison
      LightViews points_views =
          light_point_views(light->position, light->near, light->far);
      for (uint8_t v = 0; v < LIGHT_POINT_VIEWS; v++)
        glm_mat4_copy(points_views.views[v], uniform->views[v]);
    }
  }

  if (sun_list) {

    sun_uniform.length = sun_list->length;

    for (size_t i = 0; i < sun_uniform.length; i++) {

      SunLight *light = &sun_list->entries[i];
      SunLightUniform *uniform = &sun_uniform.entries[i];

      *uniform = (SunLightUniform){0};
      uniform->intensity = light->intensity;
      glm_vec3_copy(light->position, uniform->position);
      glm_vec3_copy(light->color, uniform->color);

      // get light view matrix
      LightViews sun_view = light_sun_view(light->position, light->size);
      glm_mat4_copy(sun_view.views[0], uniform->view);
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
   Bind the shadow maps and sampler to the default shader
 */
void material_texure_bind_shadow_maps(Mesh *mesh,
                                      WGPUTextureView point_texture_view,
                                      WGPUTextureView spot_texture_view) {

  const uint8_t point_map_binding = 4;
  const uint8_t directional_map_binding = 6;
  const uint8_t sampler_binding = 5;

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
          .group_index = 2,
          .entries =
              (ShaderBindGroupTextureViewEntry[]){
                  {
                      .binding = point_map_binding,
                      .texture_view = point_texture_view,
                      .dimension = WGPUTextureViewDimension_CubeArray,
                      .format = texture_format,
                      .sample_type = texture_sample_type,
                  },
                  {
                      .binding = directional_map_binding,
                      .texture_view = spot_texture_view,
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
          .group_index = 2,
          .entries =
              (ShaderBindGroupSamplerEntry[]){
                  point_sampler,
                  spot_sampler,
              },
      });
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

