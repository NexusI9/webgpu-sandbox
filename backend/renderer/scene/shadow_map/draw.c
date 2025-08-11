#include "draw.h"
#include "../backend/renderer/scene/std_pipeline/std_pipeline.h"
#include "../runtime/mesh/shader/shader.h"
#include "./core.h"
#include "webgpu/webgpu.h"

static inline void shadow_map_draw(const ShadowMapDrawDescriptor *);

static inline void
shadow_map_draw_dir_light(const ShadowMapDrawDirLightDescriptor *);

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

void shadow_map_draw(const ShadowMapDrawDescriptor *desc) {

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

  WGPUCommandEncoder shadow_encoder = desc->encoder;

  // create "local" encoder if is not included
  //(usually when only drawing one light)
  if (shadow_encoder == NULL)
    shadow_encoder = wgpuDeviceCreateCommandEncoder(desc->device, NULL);

  // create per layer texture views (depth + color)
  WGPUTextureViewDescriptor temp_layer_texture_descriptor_depth = {
      .label = "Shadow per layer texture view - Depth",
      .format = SHADOW_DEPTH_FORMAT,
      .dimension = WGPUTextureViewDimension_2D,
      .baseArrayLayer = desc->layer,
      .arrayLayerCount = 1,
      .mipLevelCount = 1,
      .baseMipLevel = 0,
  };

  WGPUTextureViewDescriptor temp_layer_texture_descriptor_color = {
      .label = "Shadow per layer texture view - Color",
      .format = SHADOW_COLOR_FORMAT,
      .dimension = WGPUTextureViewDimension_2D,
      .baseArrayLayer = desc->layer,
      .arrayLayerCount = 1,
      .mipLevelCount = 1,
      .baseMipLevel = 0,
  };

  WGPUTextureView temp_layer_texture_view_depth = wgpuTextureCreateView(
      desc->depth_texture, &temp_layer_texture_descriptor_depth);

  WGPUTextureView temp_layer_texture_view_color = wgpuTextureCreateView(
      desc->color_texture, &temp_layer_texture_descriptor_color);

  // create render pass and render it to the nested layer
  WGPURenderPassEncoder shadow_pass = wgpuCommandEncoderBeginRenderPass(
      shadow_encoder, &(WGPURenderPassDescriptor){
                          .label = "Shadow render pass encoder",
                          .colorAttachmentCount = 1,
                          .colorAttachments =
                              &(WGPURenderPassColorAttachment){
                                  .view = temp_layer_texture_view_color,
                                  .clearValue = {0.0f, 0.0f, 0.0f, 1.0f},
                                  .loadOp = WGPULoadOp_Clear,
                                  .storeOp = WGPUStoreOp_Store,
                                  .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
                              },
                          .depthStencilAttachment =
                              &(WGPURenderPassDepthStencilAttachment){
                                  .view = temp_layer_texture_view_depth,
                                  .depthClearValue = 1.0f,
                                  .depthLoadOp = WGPULoadOp_Clear,
                                  .depthStoreOp = WGPUStoreOp_Store,
                              },
                      });

  // draw target
  for (size_t i = 0; i < desc->mesh_list->length; i++) {

    Mesh *mesh = desc->mesh_list->entries[i];

    // swap pipeline (cull front/back)
    mesh_shader_shadow(mesh)->pipeline = desc->pipeline;

    // update each mesh shadow uniforms with current light view
    mesh_shader_shadow_update_view(mesh, desc->light_view);

    // draw mesh
    mesh_draw(mesh_topology_base(mesh), mesh_shader_shadow(mesh), &shadow_pass);
  }

  wgpuRenderPassEncoderEnd(shadow_pass);
  wgpuTextureViewRelease(temp_layer_texture_view_depth);
  wgpuTextureViewRelease(temp_layer_texture_view_color);

  // clean up "local" encoder if not provided in the descriptor
  if (desc->encoder == NULL) {

    // finish encoding command
    WGPUCommandBuffer command_buffer =
        wgpuCommandEncoderFinish(shadow_encoder, NULL);
    wgpuQueueSubmit(desc->queue, 1, &command_buffer);

    // clean up
    wgpuCommandBufferRelease(command_buffer);
    wgpuCommandEncoderRelease(shadow_encoder);
  }

  /*debug_view_add(&debug_view_light,
                 &(ViewDescriptor){
                     .texture_view = layer_texture_view_color,
                     .size = {1.0f, 1.0f * 9.0f / 16.0f},
                     .position = {0.0f, 0.0f},
                     });*/
}

/**
   ▗▄▄▄ ▗▄▄▖  ▗▄▖ ▗▖ ▗▖     ▗▄▖ ▗▖   ▗▖
   ▐▌  █▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌    ▐▌ ▐▌▐▌   ▐▌
   ▐▌  █▐▛▀▚▖▐▛▀▜▌▐▌ ▐▌    ▐▛▀▜▌▐▌   ▐▌
   ▐▙▄▄▀▐▌ ▐▌▐▌ ▐▌▐▙█▟▌    ▐▌ ▐▌▐▙▄▄▖▐▙▄▄▖

    Shadow map array Generation:
    - bind light view projection
    - render each mesh under lights POV
    - render to the shadow array
 */
void shadow_map_draw_all(const ShadowMapDrawAllDescriptor *desc) {

  // VERBOSE_PROCESS("Computing all shadow maps...");

  /* TODO :
     When using "global" shadow encoder and passing it through all the passes
     create an async conflict issue: the point light view probably takes longer
     to be calculated and gets overriden by the the sun point of view.

     A solution to this is to add an offset or use a command encoder per light
   */

  MeshRefList *target_mesh_list = desc->mesh_list;

  const size_t point_length = desc->lights->point.length;
  const size_t spot_length = desc->lights->spot.length;
  const size_t sun_length = desc->lights->sun.length;

  /*
  ==========================================

     I. create Point Light Shadow Mapping

  ==========================================
 */

  WGPUCommandEncoder shadow_encoder =
      wgpuDeviceCreateCommandEncoder(desc->device, NULL);

  WGPUCommandBuffer command_buffer;

  for (size_t p = 0; p < point_length; p++)
    shadow_map_draw_point_light(&(ShadowMapDrawPointLightDescriptor){
        .layer = p,
        .light = &desc->lights->point.entries[p],
        .mesh_list = desc->mesh_list,
        .device = desc->device,
        .queue = desc->queue,
        .encoder = shadow_encoder,
        .color_map = desc->lights->point.color_map,
        .depth_map = desc->lights->point.depth_map,
    });

  /*
    ==================================================

       II. create Directional Light Shadow Mapping

    ==================================================
   */

  for (size_t p = 0; p < spot_length; p++)
    shadow_map_draw_spot_light(&(ShadowMapDrawSpotLightDescriptor){
        .layer = p,
        .light = &desc->lights->spot.entries[p],
        .mesh_list = desc->mesh_list,
        .device = desc->device,
        .queue = desc->queue,
        .encoder = shadow_encoder,
        .color_map = desc->lights->spot.color_map,
        .depth_map = desc->lights->spot.depth_map,
    });

  /*
  ==========================================

     III. create Sun Light Shadow Mapping

  ==========================================
 */

  for (size_t p = 0; p < sun_length; p++)
    shadow_map_draw_sun_light(&(ShadowMapDrawSunLightDescriptor){
        // TODO: currently use spot light color_map, maybe make a linked
        // pointer
        // to the same map but include it in the sun light list struct itself.
        .color_map = desc->lights->spot.color_map,
        .depth_map = desc->lights->spot.depth_map,
        .layer = spot_length + p,
        .device = desc->device,
        .queue = desc->queue,
        .encoder = shadow_encoder,
        .mesh_list = desc->mesh_list,
        .light = &desc->lights->sun.entries[p],
    });

  // finish encoding command
  command_buffer = wgpuCommandEncoderFinish(shadow_encoder, NULL);
  wgpuQueueSubmit(desc->queue, 1, &command_buffer);

  // clean up
  wgpuCommandBufferRelease(command_buffer);
  wgpuCommandEncoderRelease(shadow_encoder);
}

/**
   ▗▄▄▄ ▗▄▄▖  ▗▄▖ ▗▖ ▗▖     ▗▄▄▖ ▗▄▖ ▗▖    ▗▄▖
   ▐▌  █▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌    ▐▌   ▐▌ ▐▌▐▌   ▐▌ ▐▌
   ▐▌  █▐▛▀▚▖▐▛▀▜▌▐▌ ▐▌     ▝▀▚▖▐▌ ▐▌▐▌   ▐▌ ▐▌
   ▐▙▄▄▀▐▌ ▐▌▐▌ ▐▌▐▙█▟▌    ▗▄▄▞▘▝▚▄▞▘▐▙▄▄▖▝▚▄▞▘

 */

void shadow_map_draw_point_light(
    const ShadowMapDrawPointLightDescriptor *desc) {

  // retrieve 6 views of point cube
  LightViews light_views = light_point_views(
      desc->light->position, desc->light->near, desc->light->far);

  // render scene and store depth map for each view
  for (size_t v = 0; v < light_views.length; v++) {

    // Render scene (create shadow render pass to texture layer)
    size_t layer = desc->layer * light_views.length + v;

    shadow_map_draw(&(ShadowMapDrawDescriptor){
        .mesh_list = desc->mesh_list,
        .color_texture = desc->color_map,
        .depth_texture = desc->depth_map,
        .layer = layer,
        .device = desc->device,
        .queue = desc->queue,
        .encoder = desc->encoder,
        .light_view = &light_views.views[v],
        .pipeline = std_pipeline(PipelineType_Shadow),
    });
  }
}

/**
   Function to handle both spot and sun light drawing
 */
void shadow_map_draw_dir_light(const ShadowMapDrawDirLightDescriptor *desc) {

  /*
   Cullmode adjustment below:
   Point Light pipeline use a FRONT CULL combined with a flip the scene on
   the x axis to match cube map coordinates.

   However since spot light use a casual Texture and doesn't require
   to flip the scene projection, we set back the cull to BACK.
   */

  // Render scene (create shadow render pass to texture layer)
  shadow_map_draw(&(ShadowMapDrawDescriptor){
      .mesh_list = desc->mesh_list,
      .color_texture = desc->color_map,
      .depth_texture = desc->depth_map,
      .layer = desc->layer,
      .device = desc->device,
      .queue = desc->queue,
      .encoder = desc->encoder,
      .light_view = &desc->views->views[0],
      .pipeline = desc->pipeline,
  });
}

void shadow_map_draw_sun_light(const ShadowMapDrawSunLightDescriptor *desc) {

  // get each light orthographic view depending on target
  LightViews light_views =
      light_sun_view(desc->light->position, desc->light->size);

  shadow_map_draw_dir_light(&(ShadowMapDrawDirLightDescriptor){
      .color_map = desc->color_map,
      .depth_map = desc->depth_map,
      .mesh_list = desc->mesh_list,
      .queue = desc->queue,
      .device = desc->device,
      .encoder = desc->encoder,
      .layer = desc->layer,
      .views = &light_views,
      .pipeline = std_pipeline(PipelineType_ShadowCullBack),
  });
}

void shadow_map_draw_spot_light(const ShadowMapDrawSpotLightDescriptor *desc) {

  // get each light orthographic view depending on target
  LightViews light_views = light_spot_view(
      desc->light->position, desc->light->target, desc->light->angle);

  shadow_map_draw_dir_light(&(ShadowMapDrawDirLightDescriptor){
      .color_map = desc->color_map,
      .depth_map = desc->depth_map,
      .mesh_list = desc->mesh_list,
      .device = desc->device,
      .queue = desc->queue,
      .encoder = desc->encoder,
      .layer = desc->layer,
      .views = &light_views,
      .pipeline = std_pipeline(PipelineType_ShadowCullBack),
  });
}
