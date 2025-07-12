#include "shadow_pass.h"
#include "../runtime/prefab/debug/view.h"
#include "../runtime/material/material.h"
#include "../runtime/texture/texture.h"
#include "../utils/math.h"
#include <string.h>

// static DebugView debug_view_light;
static inline void
shadow_pass_create_textures(const ShadowPassTextureDescriptor *);
static inline void
shadow_pass_to_texture(const ShadowPassToTextureDescriptor *);
static inline void
shadow_pass_fallback_to_texture(const ShadowPassFallbackToTextureDescriptor *);
static inline void shadow_pass_create_map(const ShadowPassMapDescriptor *);

void shadow_pass_init(Scene *scene, WGPUDevice device, WGPUQueue queue) {

  printf("==== COMPUTING SHADOW ====\n");

  /*debug_view_create(&debug_view_light, &(DebugViewCreateDescriptor){
                                           .device = &device,
                                           .queue = &queue,
                                           });*/

  // create multi layered light texture (passed to the renderpass)
  size_t point_light_length = scene->lights.point.length;
  size_t spot_light_length = scene->lights.spot.length;
  size_t sun_light_length = scene->lights.sun.length;
  /*
                               For each shadow light:

                 .-----------------.           .-----------------.
                 |  Color Texture  |           |  Depth Texture  |
                 '-----------------'           '-----------------'
                         |                              |
                 .=======|====== RENDER LIGHT POV ======|=======.
                 |       |                              |       |
                 |       |     .----- vertex ----.      |       |
                 |       |     |    Depth Pass   | <----|       |
                 |       |     '-----------------'      |       |
                 |       |              |               |       |
                 |       |     .--- fragment ---.       |       |
                 |       |---> |   Color Pass   |       |       |
                 |       |     '----------------'       |       |
                 |       |                              |       |
                 '=======|==============================|======='
                         |_____________.    .___________|
                                       |   |
                              .---------------------.
                              | Light Texture Array |
                              '---------------------'


     */

  // Setup point light
  WGPUTexture point_shadow_texture_color;
  WGPUTexture point_shadow_texture_depth;

  // Switch to minimum size if no light
  int point_shadow_texture_size =
      point_light_length > 0 ? SHADOW_MAP_SIZE : TEXTURE_MIN_SIZE;

  shadow_pass_create_textures(&(ShadowPassTextureDescriptor){
      .dimension = WGPUTextureViewDimension_CubeArray,
      .layer_count = MAX(point_light_length, 1) * LIGHT_POINT_VIEWS,
      .device = device,
      .width = point_shadow_texture_size,
      .height = point_shadow_texture_size,
      .color =
          {
              .texture = &point_shadow_texture_color,
              .texture_view = &scene->lights.point.color_map,
          },
      .depth =
          {
              .texture = &point_shadow_texture_depth,
              .texture_view = &scene->lights.point.depth_map,
          },
  });

  /*
    Setup spot & sun light
    Spot and Sun lights are all together stacked up in the same "directional
    light" texture array.
    Compared to the point lights that have a dedicated
    cubemap uniform entry.

    Another semantic to note is that Spot Lights and Sun Lights are both
    encompassed under the "Directional Light" term.

    The order is the following:

    .--------------. -----.
    |   Layer 0    |      |
    |--------------|      |
    |   Layer 1    |      |     .---------------.
    |--------------|       >----|  Spot Lights  | --.
    |   Layer 2    |      |     '--------------'    |
    |--------------|      |                         |
    |   Layer 3    | _____'                         |    .-------------------.
    |--------------| -----.                          >--| Directional Lights |
    |   Layer 4    |      |                         |   '--------------------'
    |--------------|      |     .--------------.    |
    |   Layer 5    |       >----|  Sun Lights  |----'
    |--------------|      |     '--------------'
    |   Layer 6    |      |
    '--------------' -----'


  */
  WGPUTexture spot_shadow_texture_color;
  WGPUTexture spot_shadow_texture_depth;
  int spot_shadow_texture_size = (spot_light_length + sun_light_length) > 0
                                     ? SHADOW_MAP_SIZE
                                     : TEXTURE_MIN_SIZE;

  shadow_pass_create_textures(&(ShadowPassTextureDescriptor){
      .dimension = WGPUTextureViewDimension_2DArray,
      .layer_count = MAX(spot_light_length + sun_light_length, 1),
      .device = device,
      .width = spot_shadow_texture_size,
      .height = spot_shadow_texture_size,
      .color =
          {
              .texture = &spot_shadow_texture_color,
              .texture_view = &scene->lights.spot.color_map,
          },
      .depth =
          {
              .texture = &spot_shadow_texture_depth,
              .texture_view = &scene->lights.spot.depth_map,
          },
  });

  // Generate Shadow maps (both color and depth map)
  shadow_pass_create_map(&(ShadowPassMapDescriptor){
      .scene = scene,
      .device = device,
      .queue = queue,
      .point_light =
          {
              .color_texture = &point_shadow_texture_color,
              .depth_texture = &point_shadow_texture_depth,
          },
      .directional_light =
          {
              .color_texture = &spot_shadow_texture_color,
              .depth_texture = &spot_shadow_texture_depth,
          },
  });

  // Transfer depth texture array to each meshes default shader
  for (size_t m = 0; m < scene->pipelines.lit.length; m++) {

    Mesh *current_mesh = scene->pipelines.lit.entries[m];

    // bind point & spot light texture view + sampler to Textue Shader
#ifdef RENDER_SHADOW_AS_COLOR
    const WGPUTextureView point_map = scene->lights.point.color_map;
    const WGPUTextureView spot_map = scene->lights.directional.color_map;
#else
    const WGPUTextureView point_map = scene->lights.point.depth_map;
    const WGPUTextureView spot_map = scene->lights.spot.depth_map;
#endif

    material_texure_bind_shadow_maps(current_mesh, point_map, spot_map);
  }

  // !!DEBUG: Add views to scene
  /*for (size_t v = 0; v < debug_view_length(&debug_view_light); v++) {
    mesh *view = scene_new_mesh_unlit(scene);
    mesh *view_mesh = &debug_view_light.mesh[v];
    memcpy(view, view_mesh, sizeof(mesh));
    }*/
}

/**
   Render a fallback texture to the color and depth shadow.
   Used as a fallback if no light are in the scene, simply populate the textures
   width a fallback value.
 */
void shadow_pass_fallback_to_texture(
    const ShadowPassFallbackToTextureDescriptor *desc) {

  const int width = TEXTURE_MIN_SIZE;
  const int height = TEXTURE_MIN_SIZE;
  const int8_t channels = TEXTURE_CHANNELS_RGBA;

  Texture texture;

  // create fallback texture
  texture_create(&texture, &(TextureCreateDescriptor){
                               .channels = channels,
                               .width = width,
                               .height = height,
                               .value = 0,
                           });

  const WGPUTextureDataLayout texture_layout = {
      .offset = 0,
      .bytesPerRow = width * channels,
      .rowsPerImage = height,
  };

  const WGPUExtent3D texture_dimension = {
      .width = width,
      .height = height,
      .depthOrArrayLayers = 1,
  };

  // write to color texture
  wgpuQueueWriteTexture(desc->queue,
                        &(WGPUImageCopyTexture){
                            .texture = desc->color_texture,
                            .mipLevel = 0,
                            .origin = {0, 0, 0},
                            .aspect = WGPUTextureAspect_All,
                        },
                        texture.data, texture.size, &texture_layout,
                        &texture_dimension);

  // do not need to write to depth texture (automatically set by GPU)
}

/**
   The building shadow phase is segmented in numerous steps:

   1. BINDING:
   For each scene point and spot lights, we bind the relative views to
   the mesh shadow shader view matrix uniform

   2. DRAWING:
   Once the view matrix is bound, the mesh is ready to drawn with its shadow
  shader. For each light views, each mesh will be renderder (under a certain
  point of view)

   3. STORING:
  The rendered mesh will be stored in a dedicated Texture that will be held in a
  arrayed texture in the Light list

  4. TRANSFERT
  Once the rendered textures are stored, they will be uploaded in the mesh
  "default shader" as a texture and sampler to be read and reused for calculate
  the shadow


  */

void shadow_pass_to_texture(const ShadowPassToTextureDescriptor *desc) {

  /*  Create a new "nested" texture view for each layer that points back to the
     texture。Both "global Texture view" and "indexed Texture view" point toward
                               the same texture

                           .----------------------.
                           |      WGPUTexture     |
                           '----------------------'
                   .-------------------'-------------------.
                   |                                       |
   .----------------------------------.   .-------------------------------.
   |    WGPUTextureView<all layers>   |   |    WGPUTextureView<layer N>   |
   '----------------------------------'   '-------------------------------'
     Final Texture with all layers         Target a specific texture layer

                   |                                      |
                   |                                      |
                   |		 	 .-------------------------------.
                   |			 |          1.Renderpass         |
                   |			 '-------------------------------'
                   |                         Render to specific layer
                   |
                   |
    .-------------------------------.
    |  2. Shader<texture 2D array>  |
    '-------------------------------'
        Bind the Array to shader


        In our case we create 2 layer views: one for color, and another for
      depth. The color one will mostly be used for debugging purpose, whereas
           the depth one will be used for comparison in texture shader

   */

  WGPUTextureViewDescriptor layer_texture_descriptor_depth = {
      .label = "Shadow per layer texture view - Depth",
      .format = SHADOW_DEPTH_FORMAT,
      .dimension = WGPUTextureViewDimension_2D,
      .baseArrayLayer = desc->layer,
      .arrayLayerCount = 1,
      .mipLevelCount = 1,
      .baseMipLevel = 0,
  };

  WGPUTextureViewDescriptor layer_texture_descriptor_color = {
      .label = "Shadow per layer texture view - Color",
      .format = SHADOW_COLOR_FORMAT,
      .dimension = WGPUTextureViewDimension_2D,
      .baseArrayLayer = desc->layer,
      .arrayLayerCount = 1,
      .mipLevelCount = 1,
      .baseMipLevel = 0,
  };

  WGPUTextureView layer_texture_view_depth = wgpuTextureCreateView(
      desc->depth_texture, &layer_texture_descriptor_depth);

  WGPUTextureView layer_texture_view_color = wgpuTextureCreateView(
      desc->color_texture, &layer_texture_descriptor_color);

  // create render pass and render it to the nested layer

  WGPURenderPassEncoder shadow_pass = wgpuCommandEncoderBeginRenderPass(
      desc->encoder, &(WGPURenderPassDescriptor){
                         .label = "Shadow render pass encoder",
                         .colorAttachmentCount = 1,
                         .colorAttachments =
                             &(WGPURenderPassColorAttachment){
                                 .view = layer_texture_view_color,
                                 .clearValue = {0.0f, 0.0f, 0.0f, 1.0f},
                                 .loadOp = WGPULoadOp_Clear,
                                 .storeOp = WGPUStoreOp_Store,
                                 .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
                             },
                         .depthStencilAttachment =
                             &(WGPURenderPassDepthStencilAttachment){
                                 .view = layer_texture_view_depth,
                                 .depthClearValue = 1.0f,
                                 .depthLoadOp = WGPULoadOp_Clear,
                                 .depthStoreOp = WGPUStoreOp_Store,
                             },
                     });

  scene_draw_shadow(desc->scene, &shadow_pass);

  wgpuRenderPassEncoderEnd(shadow_pass);

  /*debug_view_add(&debug_view_light,
                 &(ViewDescriptor){
                     .texture_view = layer_texture_view_color,
                     .size = {1.0f, 1.0f * 9.0f / 16.0f},
                     .position = {0.0f, 0.0f},
                     });*/
}

/**
    Shadow map array Generation:
    - bind light view projection
    - render each mesh under lights POV
    - render to the shadow array
 */
void shadow_pass_create_map(const ShadowPassMapDescriptor *desc) {

  WGPUCommandEncoder shadow_encoder =
      wgpuDeviceCreateCommandEncoder(desc->device, NULL);

  MeshRefList *target_mesh_list = &desc->scene->pipelines.lit;

  const size_t point_length = desc->scene->lights.point.length;
  const size_t spot_length = desc->scene->lights.spot.length;
  const size_t sun_length = desc->scene->lights.sun.length;

  /*
  ==========================================

     I. create Point Light Shadow Mapping

  ==========================================
 */

  // load fallback texture if no point light in scene
  if (point_length == 0) {
    for (size_t v = 0; v < LIGHT_POINT_VIEWS; v++) {
      shadow_pass_fallback_to_texture(&(ShadowPassFallbackToTextureDescriptor){
          .color_texture = *desc->point_light.color_texture,
          .depth_texture = *desc->point_light.depth_texture,
          .layer = v,
          .queue = desc->queue,
      });
    }
  }

  // create shadow shader
  for (int m = 0; m < target_mesh_list->length; m++) {
    Mesh *current_mesh = target_mesh_list->entries[m];
    // create mesh shadow shader
    mesh_create_shadow_shader(current_mesh);
  }

  for (size_t p = 0; p < point_length; p++) {

    PointLight *light = &desc->scene->lights.point.entries[p];
    // retrieve 6 views of point cube
    LightViews light_views =
        light_point_views(light->position, light->near, light->far);

    // render scene and store depth map for each view
    for (size_t v = 0; v < light_views.length; v++) {
      mat4 *current_view = &light_views.views[v];

      for (int m = 0; m < target_mesh_list->length; m++) {
        // 1. Bind meshes
        Mesh *current_mesh = target_mesh_list->entries[m];
        // bind light respective views
        material_shadow_bind_views(current_mesh, current_view);
        // build shadow shader layout
        mesh_build(current_mesh, mesh_shader_shadow(current_mesh));
      }

      // 2. Render scene (create shadow render pass to texture layer)
      size_t layer = p * light_views.length + v;
      shadow_pass_to_texture(&(ShadowPassToTextureDescriptor){
          .scene = desc->scene,
          .color_texture = *desc->point_light.color_texture,
          .depth_texture = *desc->point_light.depth_texture,
          .layer = layer,
          .device = &desc->device,
          .encoder = shadow_encoder,
      });

      // 3. Clear meshes bind group
      for (int m = 0; m < target_mesh_list->length; m++) {
        Mesh *current_mesh = target_mesh_list->entries[m];
        material_shadow_clear_bindings(current_mesh);
        // destroy previous pipeline for next views
        pipeline_destroy(shader_pipeline(mesh_shader_shadow(current_mesh)));
      }
    }
  }

  /*
    ==================================================

       II. create Directional Light Shadow Mapping

    ==================================================
   */

  if (spot_length == 0 && sun_length == 0) {
    shadow_pass_fallback_to_texture(&(ShadowPassFallbackToTextureDescriptor){
        .color_texture = *desc->directional_light.color_texture,
        .depth_texture = *desc->directional_light.depth_texture,
        .layer = 0,
        .queue = desc->queue,
    });
  }

  for (size_t p = 0; p < spot_length; p++) {

    SpotLight *light = &desc->scene->lights.spot.entries[p];

    // get each light orthographic view depending on target
    LightViews light_views =
        light_spot_view(light->position, light->target, light->angle);

    // 1. Bind meshes
    for (int m = 0; m < target_mesh_list->length; m++) {
      Mesh *current_mesh = target_mesh_list->entries[m];
      material_shadow_bind_views(current_mesh, &light_views.views[0]);

      /*
        Cullmode adjustment below:
        Point Light pipeline use a FRONT cull cause by flipping the scene on
        the x axis to match cube map coordinates.

        However since spot light use a casual Texture and doesn't require
        to flip the scene projection, we set back the cull to BACK.
      */

      material_shadow_set_cullmode(current_mesh, WGPUCullMode_Back);
      mesh_build(current_mesh, mesh_shader_shadow(current_mesh));
    }

    // 2. Render scene (create shadow render pass to texture layer)
    shadow_pass_to_texture(&(ShadowPassToTextureDescriptor){
        .scene = desc->scene,
        .color_texture = *desc->directional_light.color_texture,
        .depth_texture = *desc->directional_light.depth_texture,
        .layer = p,
        .device = &desc->device,
        .encoder = shadow_encoder,
    });

    // 3. Clear meshes bind group
    for (int m = 0; m < target_mesh_list->length; m++) {
      Mesh *current_mesh = target_mesh_list->entries[m];
      material_shadow_clear_bindings(current_mesh);
    }
  }

  /*
  ==========================================

     III. create Sun Light Shadow Mapping

  ==========================================
 */

  for (size_t p = 0; p < sun_length; p++) {

    SunLight *light = &desc->scene->lights.sun.entries[p];

    // get each light orthographic view depending on target
    LightViews light_views = light_sun_view(light->position, light->size);

    // 1. Bind meshes
    for (int m = 0; m < target_mesh_list->length; m++) {
      Mesh *current_mesh = target_mesh_list->entries[m];
      material_shadow_bind_views(current_mesh, &light_views.views[0]);

      // update cull mode due to point light using front (see Spot light pass
      // above for more detail)
      material_shadow_set_cullmode(current_mesh, WGPUCullMode_Back);
      mesh_build(current_mesh, mesh_shader_shadow(current_mesh));
    }

    // 2. Render scene (create shadow render pass to texture layer)
    shadow_pass_to_texture(&(ShadowPassToTextureDescriptor){
        .scene = desc->scene,
        .color_texture = *desc->directional_light.color_texture,
        .depth_texture = *desc->directional_light.depth_texture,
        .layer = spot_length + p,
        .device = &desc->device,
        .encoder = shadow_encoder,
    });

    // 3. Clear meshes bind group
    for (int m = 0; m < desc->scene->pipelines.lit.length; m++) {
      Mesh *current_mesh = target_mesh_list->entries[m];
      material_shadow_clear_bindings(current_mesh);
    }
  }

  // finish encoding command
  WGPUCommandBuffer command_buffer =
      wgpuCommandEncoderFinish(shadow_encoder, NULL);
  wgpuQueueSubmit(desc->queue, 1, &command_buffer);

  // clean up
  wgpuCommandBufferRelease(command_buffer);
  wgpuCommandEncoderRelease(shadow_encoder);
}

/**
   Create the two shadow textures (color and depth) for the point lights
   In our semantic we use both terms Maps and Textures, however they both
   serve different purpose.
   - The Shadow Texture holds the Shadow mapping.
   - The Shadow map is the result from our rendering.
 */
void shadow_pass_create_textures(const ShadowPassTextureDescriptor *desc) {

  // texture
  WGPUTextureDescriptor texture_descriptor_base = {
      .size =
          (WGPUExtent3D){
              .width = desc->width,
              .height = desc->height,
              .depthOrArrayLayers = desc->layer_count,
          },
      .usage = WGPUTextureUsage_CopyDst | WGPUTextureUsage_RenderAttachment |
               WGPUTextureUsage_TextureBinding,
      .dimension = WGPUTextureDimension_2D,
      .mipLevelCount = 1,
      .sampleCount = 1,
  };

  WGPUTextureDescriptor texture_descriptor_color = texture_descriptor_base;
  texture_descriptor_color.label = "Light shadow texture - Color";
  texture_descriptor_color.format = SHADOW_COLOR_FORMAT;

  WGPUTextureDescriptor texture_descriptor_depth = texture_descriptor_base;
  texture_descriptor_depth.label = "Light shadow texture - Depth";
  texture_descriptor_depth.format = SHADOW_DEPTH_FORMAT;

  // setup global texture view
  WGPUTextureViewDescriptor texture_view_descriptor_base = {
      .label = "Light Shadow: global texture view - Depth",
      .format = SHADOW_DEPTH_FORMAT,
      .dimension = desc->dimension,
      .mipLevelCount = 1,
      .baseMipLevel = 0,
      .arrayLayerCount = desc->layer_count,
      .baseArrayLayer = 0,
      .aspect = WGPUTextureAspect_DepthOnly,
  };

  // Create color texture
  *desc->color.texture =
      wgpuDeviceCreateTexture(desc->device, &texture_descriptor_color);
  *desc->color.texture_view = wgpuTextureCreateView(
      *desc->color.texture,
      &(WGPUTextureViewDescriptor){
          .label = "Light Shadow: global texture view - Color",
          .format = SHADOW_COLOR_FORMAT,
          .dimension = desc->dimension,
          .mipLevelCount = 1,
          .baseMipLevel = 0,
          .arrayLayerCount = desc->layer_count,
          .baseArrayLayer = 0,
          .aspect = WGPUTextureAspect_Undefined,
      });

  // Setup light depth texture

  // Create depth texture
  *desc->depth.texture =
      wgpuDeviceCreateTexture(desc->device, &texture_descriptor_depth);
  *desc->depth.texture_view = wgpuTextureCreateView(
      *desc->depth.texture,
      &(WGPUTextureViewDescriptor){
          .label = "Light Shadow: global texture view - Depth",
          .format = SHADOW_DEPTH_FORMAT,
          .dimension = desc->dimension,
          .mipLevelCount = 1,
          .baseMipLevel = 0,
          .arrayLayerCount = desc->layer_count,
          .baseArrayLayer = 0,
          .aspect = WGPUTextureAspect_DepthOnly,
      });
}
