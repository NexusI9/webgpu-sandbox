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

  // printf("pass depth texture: %p\n", )

  WGPUTextureView temp_layer_texture_view_depth = wgpuTextureCreateView(
      desc->pass->depth.texture, &temp_layer_texture_descriptor_depth);

  WGPUTextureView temp_layer_texture_view_color = wgpuTextureCreateView(
      desc->pass->color.texture, &temp_layer_texture_descriptor_color);

  render_pass_update_preprocessor_data(desc->pass, 0,
                                       &(LightShadowData){
                                           .pipeline = desc->pipeline,
                                           .light_view = desc->light_view,
                                       });

  render_pass_draw(desc->pass, &(RenderPassViewOverride){
                                   .color = temp_layer_texture_view_color,
                                   .depth = temp_layer_texture_view_depth,
                               });

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

     The overall flow would be:

     1. set the shadow pipeline minBindSize to:
     .------------------------------------------------------------------------.
     |                      Mp * Nvp + Ms + Mn * sizeof(mat4)                 |
     |	               ex:  16 *  6  + 16 + 16 *    64                        |
     |                                                                        |
     |  - Mp: max point light                                                 |
     |  - Nvp: number of point light view (6)                                 |
     |  - Ms: max spot light                                                  |
     |  - Mn: max sun light                                                   |
     '------------------------------------------------------------------------'

     2. For each light: push the view to the ViewArray at correct index (cache?)
     .------------------------------------------------------------------------.
     |  Offset 0 × sizeof(mat4)       → point light view matrix face 0        |
     |  Offset 1 × sizeof(mat4)       → point light view matrix face 1        |
     |  ...                                                                   |
     |  Offset 5 × sizeof(mat4)       → point light view matrix face 5        |
     |  Offset 6 × sizeof(mat4)       → sun light view matrix                 |
     |  ...                                                                   |
     '------------------------------------------------------------------------'

      3. Write to each mesh shadow view uniform the views with the
      index * sizeof(mat4) as offset. (shader_update_uniform)

      4. Draw the mesh with the same offset (wgpuRenderPassEncoderSetBindGroup)


   */

  MeshRefList *target_mesh_list = desc->mesh_list;

  const size_t point_length = desc->lights->point.shadow.length;
  const size_t spot_length = desc->lights->spot.shadow.length;
  const size_t sun_length = desc->lights->sun.shadow.length;

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
        .light = desc->lights->point.shadow.entries[p],
        .pass = &desc->lights->point.shadow.pass,
        .device = desc->device,
        .queue = desc->queue,
        .encoder = NULL, // shadow_encoder,
    });

  /*
    ==================================================

       II. create Directional Light Shadow Mapping

    ==================================================
   */

  for (size_t p = 0; p < spot_length; p++)
    shadow_map_draw_spot_light(&(ShadowMapDrawSpotLightDescriptor){
        .layer = p,
        .light = desc->lights->spot.shadow.entries[p],
        .device = desc->device,
        .queue = desc->queue,
        .encoder = NULL,
        .pass = &desc->lights->spot.shadow.pass,
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
        .layer = spot_length + p,
        .device = desc->device,
        .queue = desc->queue,
        .encoder = shadow_encoder,
        .pass = &desc->lights->spot.shadow.pass,
        .light = desc->lights->sun.shadow.entries[p],
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
  Projection light_views;
  projection_point(&light_views, desc->light->position, desc->light->near,
                   desc->light->far);

  // render scene and store depth map for each view
  for (size_t v = 0; v < light_views.length; v++) {

    // Render scene (create shadow render pass to texture layer)
    size_t layer = desc->layer * light_views.length + v;

    shadow_map_draw(&(ShadowMapDrawDescriptor){
        .pass = desc->pass,
        .layer = layer,
        .device = desc->device,
        .queue = desc->queue,
        .encoder = desc->encoder,
        .light_view = &light_views.combined[v],
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
  for (size_t v = 0; v < desc->views->length; v++)
    shadow_map_draw(&(ShadowMapDrawDescriptor){
        .pass = desc->pass,
        .layer = desc->layer,
        .device = desc->device,
        .queue = desc->queue,
        .encoder = desc->encoder,
        .light_view = &desc->views->combined[v],
        .pipeline = desc->pipeline,
    });
}

void shadow_map_draw_sun_light(const ShadowMapDrawSunLightDescriptor *desc) {

  // get each light orthographic view depending on target
  Projection light_views;
  projection_sun(&light_views, desc->light->position, desc->light->size);

  shadow_map_draw_dir_light(&(ShadowMapDrawDirLightDescriptor){
      .pass = desc->pass,
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
  Projection light_views;
  projection_spot(&light_views, desc->light->position, desc->light->target,
                  desc->light->angle);

  shadow_map_draw_dir_light(&(ShadowMapDrawDirLightDescriptor){
      .pass = desc->pass,
      .device = desc->device,
      .queue = desc->queue,
      .encoder = desc->encoder,
      .layer = desc->layer,
      .views = &light_views,
      .pipeline = std_pipeline(PipelineType_ShadowCullBack),
  });
}
