#include "material.h"
#include "mesh.h"

/**
   Bind Mesh, Camera and Projection matrix to a given mesh shader
   Note that the binding process follows a fixed convention of order, meaning
   one shall ensure the shader actually fits the bellow binding order:
   - Binding 0: Viewport projection matrix
   - Binding 1: Camera matrix
   - Binding 2: Model matrix
 */
void material_texture_bind_views(mesh *mesh, camera *camera, viewport *viewport,
                                 uint8_t group_index) {

  ShaderViewProjectionUniform proj_view_data;

  CameraUniform uCamera = camera_uniform(camera);
  ViewportUniform uViewport = viewport_uniform(viewport);
  MeshUniform uMesh = mesh_model_uniform(mesh);

  ShaderBindGroupEntry entries[3] = {
      // viewport
      {
          .binding = 0,
          .data = &uViewport,
          .size = sizeof(ViewportUniform),
          .offset = 0,
      },
      // camera
      {
          .binding = 1,
          .data = &uCamera,
          .size = sizeof(CameraUniform),
          .offset = 0,
          .update_callback = camera_update_matrix_uniform,
          .update_data = camera,
      },
      // model
      {
          .binding = 2,
          .data = &uMesh,
          .size = sizeof(MeshUniform),
          .offset = 0,
      },
  };

  shader_add_uniform(
      &mesh->shader.texture,
      &(ShaderCreateUniformDescriptor){
          .group_index = group_index,
          .entry_count = 3,
          .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
          .entries = entries,
      });

  for (size_t c = 0; c < mesh->children.length; c++)
    material_texture_bind_views(&mesh->children.items[c], camera, viewport,
                                group_index);
}

/**
   Init light list uniforms
   due to WGSL array uniforms necessity to have constant size, uniforms lists
   are already set at <light_list>[12], init them all to 0
   by default we will upload all the lights (point, ambient, directional)
   within a defined group
  */
void material_texture_bind_lights(mesh *mesh, viewport *viewport,
                                  AmbientLightList *ambient_list,
                                  DirectionalLightList *directional_list,
                                  PointLightList *point_list,
                                  uint8_t group_index) {

  AmbientLightListUniform ambient_uniform;
  DirectionalLightListUniform directional_uniform;
  PointLightListUniform point_uniform;

  if (ambient_list) {
    // update length
    ambient_uniform.length = ambient_list->length;
    // update entries
    for (size_t i = 0; i < ambient_uniform.length; i++) {
      AmbientLight *light = &ambient_list->items[i];
      AmbientLightUniform *uniform = &ambient_uniform.items[i];

      // map light to light uniform (including paddings...)
      *uniform = (AmbientLightUniform){0};
      uniform->intensity = light->intensity;
      glm_vec3_copy(light->color, uniform->color);
    }
  }

  if (directional_list) {
    // update length
    directional_uniform.length = directional_list->length;
    // update entries
    for (size_t i = 0; i < directional_uniform.length; i++) {
      DirectionalLight *light = &directional_list->items[i];
      DirectionalLightUniform *uniform = &directional_uniform.items[i];

      *uniform = (DirectionalLightUniform){0};
      uniform->intensity = light->intensity;
      glm_vec3_copy(light->color, uniform->color);
      glm_vec3_copy(light->position, uniform->position);
      glm_vec3_copy(light->target, uniform->target);
    }
  }

  if (point_list) {
    // update length
    point_uniform.length = point_list->length;
    // update entries
    for (size_t i = 0; i < point_uniform.length; i++) {
      PointLight *light = &point_list->items[i];
      PointLightUniform *uniform = &point_uniform.items[i];

      *uniform = (PointLightUniform){0};
      uniform->intensity = light->intensity;
      glm_vec3_copy(light->color, uniform->color);
      glm_vec3_copy(light->position, uniform->position);

      // copy 6 points views for shader depth comparison
      PointLightViews points_views =
          light_point_views(light->position, viewport);
      for (uint8_t v = 0; v < LIGHT_POINT_VIEWS; v++) {
        // glm_mat4_transpose(points_views.views[v]);
        glm_mat4_copy(points_views.views[v], uniform->views[v]);
        // print_mat4(uniform->views[v]);
        // printf("----\n");
      }
    }
  }

  ShaderBindGroupEntry entries[3] = {
      // ambient light
      {
          .binding = 0,
          .data = &ambient_uniform,
          .offset = 0,
          .size = sizeof(AmbientLightListUniform),
      },
      // directional light
      {
          .binding = 1,
          .data = &directional_uniform,
          .offset = 0,
          .size = sizeof(DirectionalLightListUniform),
      },
      // point light
      {
          .binding = 2,
          .data = &point_uniform,
          .offset = 0,
          .size = sizeof(PointLightListUniform),
      },
  };

  shader_add_uniform(
      &mesh->shader.texture,
      &(ShaderCreateUniformDescriptor){
          .group_index = group_index,
          .entry_count = 3,
          .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
          .entries = entries,
      });

  for (size_t c = 0; c < mesh->children.length; c++)
    material_texture_bind_lights(&mesh->children.items[c], viewport,
                                 ambient_list, directional_list, point_list,
                                 group_index);
}

/**
   Clear the bind groups of mesh and children
   Basically an alias of shader method but include the reccursion for childrens
 */
void material_clear_bindings(mesh *mesh, MeshDrawMethod method) {

  shader_bind_group_clear(mesh_select_shader(mesh, method));

  for (size_t c = 0; c < mesh->children.length; c++)
    material_clear_bindings(&mesh->children.items[c], method);
}

/**
   Bind a specific point light view to the mesh's shadow shader
   The function is called during the scene shadow updating process
   As to provide to the shadow shader each lights views.

   Note that the view matrix shall be combination of the [projection view] *
   [light view] already multiplied together as there is currently no need to
   upload separate views in the shader.
 */
void material_shadow_bind_views(mesh *mesh, mat4 *view) {

  shader_add_uniform(&mesh->shader.shadow,
                     &(ShaderCreateUniformDescriptor){
                         .group_index = 0,
                         .entry_count = 1,
                         .visibility = WGPUShaderStage_Vertex,
                         .entries =
                             (ShaderBindGroupEntry[]){
                                 {
                                     .binding = 0,
                                     .data = view,
                                     .size = sizeof(mat4),
                                     .offset = 0,
                                 },
                             },
                     });

  for (size_t c = 0; c < mesh->children.length; c++)
    material_shadow_bind_views(&mesh->children.items[c], view);
}

/**
   Bind the shadow maps and sampler to the default shader
 */
void material_shadow_bind_maps(mesh *mesh, WGPUTextureView *shadow_texture) {

  // add multi-layered texture to default shader
  shader_add_texture_view(
      mesh_shader_texture(mesh),
      &(ShaderCreateTextureViewDescriptor){
          .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
          .entry_count = 1,
          .group_index = 2,
          .entries = (ShaderBindGroupTextureViewEntry[]){{
              .binding = 3,
              .texture_view = *shadow_texture,
              .dimension = WGPUTextureViewDimension_2DArray,
              .format =
                  WGPUTextureFormat_BGRA8Unorm, // WGPUTextureFormat_Depth32Float,
              .sample_type =
                  WGPUTextureSampleType_Float // WGPUTextureSampleType_Depth,
          }},
      });

  // add related sampler to default shader
  // NOTE: With depth texture need to use a special sampler type: Comparison
  shader_add_sampler(
      mesh_shader_texture(mesh),
      &(ShaderCreateSamplerDescriptor){
          .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
          .entry_count = 1,
          .group_index = 2,
          .entries = (ShaderBindGroupSamplerEntry[]){{
              .binding = 4,
              .type =
                  WGPUSamplerBindingType_Filtering, // WGPUSamplerBindingType_Comparison,
              .addressModeU = WGPUAddressMode_ClampToEdge,
              .addressModeV = WGPUAddressMode_ClampToEdge,
              .addressModeW = WGPUAddressMode_ClampToEdge,
              .magFilter = WGPUFilterMode_Linear,
              .minFilter = WGPUFilterMode_Linear,
              .compare =
                  WGPUCompareFunction_Undefined // WGPUCompareFunction_LessEqual,
          }},
      });

  for (size_t c = 0; c < mesh->children.length; c++)
    material_shadow_bind_maps(&mesh->children.items[c], shadow_texture);
}
