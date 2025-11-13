#include "core.h"

#include "backend/context.h"
#include "backend/logger.h"
#include "backend/resource_manager.h"
#include "runtime/light/core.h"
#include "runtime/light/list.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/pipeline/render.h"
#include "backend/renderer/render_pass/core.h"
#include "runtime/shader/update.h"
#include "runtime/texture/core.h"
#include "utils/math.h"
#include "webgpu/webgpu.h"

static inline void
shadow_map_create_render_pass(RenderPass *, WGPUTexture, WGPUTexture,
                              WGPUTextureView, WGPUTextureView,
                              const TextureResolution, const TextureResolution,
                              const RenderPassLayoutListDescriptor *);

/**
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

void shadow_map_init(const ShadowMapInitDescriptor *desc) {

  logger_add(LoggerFlag_Process, "Creating scene shadow map textures...");

  // create multi layered light texture (passed to the renderpass)
  const size_t point_light_length = desc->lights->point.shadow.length;
  const size_t spot_light_length = desc->lights->spot.shadow.length;
  const size_t sun_light_length = desc->lights->sun.shadow.length;

  struct {
    const size_t layer_count;
    const WGPUTextureViewDimension dimension;
    RenderPass *pass;
  } light_config[] = {
      {
          MAX(point_light_length, LIGHT_MAX_CAPACITY) * LIGHT_POINT_VIEWS,
          WGPUTextureViewDimension_CubeArray,
          &desc->lights->point.shadow.pass,
      },
      {
          MAX(spot_light_length + sun_light_length, LIGHT_MAX_CAPACITY),
          WGPUTextureViewDimension_2DArray,
          &desc->lights->spot.shadow.pass,
      },
  };

  for (size_t i = 0; i < sizeof(light_config) / sizeof(light_config[0]); i++) {
    WGPUTexture color_texture, depth_texture;
    WGPUTextureView color_view, depth_view;
    shadow_pass_texture_create(&(ShadowPassTextureDescriptor){
        .dimension = light_config[i].dimension,
        .layer_count = light_config[i].layer_count,
        .width = SHADOW_MAP_SIZE,
        .height = SHADOW_MAP_SIZE,
        .color_texture = &color_texture,
        .color_view = &color_view,
        .depth_texture = &depth_texture,
        .depth_view = &depth_view,
    });

    shadow_map_create_render_pass(
        light_config[i].pass, color_texture, depth_texture, color_view,
        depth_view, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, desc->draw_list);
  }
}

/**
   Create the two shadow textures (color and depth) for the point lights
   In our semantic we use both terms Maps and Textures, however they both
   serve different purpose.
   - The Shadow Texture holds the Shadow mapping.
   - The Shadow map is the result from our rendering.
 */
void shadow_pass_texture_create(const ShadowPassTextureDescriptor *desc) {

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

  // Create color texture
  WGPUTexture color_texture = rem_new_texture(&texture_descriptor_color);

  if (desc->color_texture)
    *desc->color_texture = color_texture;

  WGPUTextureView color_view = rem_new_view(
      color_texture, &(WGPUTextureViewDescriptor){
                         .label = "Light Shadow: global texture view - Color",
                         .format = SHADOW_COLOR_FORMAT,
                         .dimension = desc->dimension,
                         .baseMipLevel = 0,
                         .mipLevelCount = 1,
                         .baseArrayLayer = 0,
                         .arrayLayerCount = desc->layer_count,
                         .aspect = WGPUTextureAspect_Undefined,
                     });

  if (desc->color_view)
    *desc->color_view = color_view;

  // Setup light depth texture

  // Create depth texture
  WGPUTexture depth_texture = rem_new_texture(&texture_descriptor_depth);

  if (desc->depth_texture)
    *desc->depth_texture = depth_texture;

  WGPUTextureView depth_view = rem_new_view(
      depth_texture, &(WGPUTextureViewDescriptor){
                         .label = "Light Shadow: global texture view - Depth",
                         .dimension = desc->dimension,
                         .format = SHADOW_DEPTH_FORMAT,
                         .baseMipLevel = 0,
                         .mipLevelCount = 1,
                         .baseArrayLayer = 0,
                         .arrayLayerCount = desc->layer_count,
                         .aspect = WGPUTextureAspect_DepthOnly,
                     });

  if (desc->depth_view)
    *desc->depth_view = depth_view;
}

void shadow_map_pass_preprocessor_callback(const RenderPass *pass, Mesh *mesh,
                                           void *userData) {

  LightShadowData *data = (LightShadowData *)userData;

  // DEBUG
  printf("Mesh: %s \n", mesh->name);
  printf("userData: %p", userData);
  printf("View offset: %lu\n", data->view_offset);
  
  // update each mesh shadow uniforms with current light view
  shader_update_bind_group_offset(mesh_shader(mesh, MeshShader_Shadow), 0, 0,
                                  data->view_offset, ShaderUpdateFlag_None);
}

void shadow_pass_update_resolution(RenderPass *pass,
                                   const TextureResolution resolution,
                                   const WGPUTextureViewDimension dimension) {

  if (resolution == wgpuTextureGetWidth(pass->color.texture) ||
      resolution == wgpuTextureGetHeight(pass->color.texture))
    return;

  uint32_t layer_count = wgpuTextureGetDepthOrArrayLayers(pass->color.texture);

  {
    // === clean up ===
      rem_destroy_texture(&pass->color.texture);
      rem_destroy_texture(&pass->depth.texture);
    
      rem_destroy_view(&pass->color.views[0]);
      rem_destroy_view(&pass->depth.views[0]);
  }

  shadow_pass_texture_create(&(ShadowPassTextureDescriptor){
      .dimension = dimension,
      .layer_count = layer_count,
      .width = resolution,
      .height = resolution,
      .color_texture = &pass->color.texture,
      .color_view = &pass->color.views[0],
      .depth_texture = &pass->depth.texture,
      .depth_view = &pass->depth.views[0],
  });

  pass->color.attachment.view = pass->color.views[0];
  pass->depth.attachment.view = pass->depth.views[0];
}

void shadow_map_create_render_pass(
    RenderPass *pass, WGPUTexture color_texture, WGPUTexture depth_texture,
    WGPUTextureView color_view, WGPUTextureView depth_view,
    const TextureResolution width, const TextureResolution height,
    const RenderPassLayoutListDescriptor *draw_list) {

  // set render pass
  render_pass_create(
      pass, &(RenderPassCreateDescriptor){
                .type = RenderPassType_OffScreen,
                .label = "Shadow Map Pass",
                .color =
                    &(RenderPassColorAttachment){
                        .texture = color_texture,
                        .attachment =
                            {
                                .view = color_view,
                                .clearValue = {0.0f, 0.0f, 0.0f, 1.0f},
                                .loadOp = WGPULoadOp_Clear,
                                .storeOp = WGPUStoreOp_Store,
                                .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
                            },
                    },
                .depth =
                    &(RenderPassDepthAttachment){
                        .texture = depth_texture,
                        .attachment =
                            {
                                .view = depth_view,
                                .depthClearValue = 1.0f,
                                .depthLoadOp = WGPULoadOp_Clear,
                                .depthStoreOp = WGPUStoreOp_Store,
                            },
                    },
                .height = height,
                .width = width,
                .multisample = PipelineMultisampleCount_1x,
                .draw_list = draw_list,
            });
}
