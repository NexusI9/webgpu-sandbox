#include "core.h"
#include "../runtime/mesh/shader/shader.h"
#include "../runtime/prefab/debug/view.h"
#include "../runtime/scene/draw.h"
#include "../runtime/texture/texture.h"
#include "../utils/math.h"
#include "../utils/system.h"
#include "./draw.h"
#include <string.h>

// static DebugView debug_view_light;
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

  VERBOSE_PROCESS("Creating scene shadow map textures...");

  /*debug_view_create(&debug_view_light, &(DebugViewCreateDescriptor){
                                           .device = &device,
                                           .queue = &queue,
                                           });*/

  // create multi layered light texture (passed to the renderpass)
  size_t point_light_length = desc->lights->point.shadow.length;
  size_t spot_light_length = desc->lights->spot.shadow.length;
  size_t sun_light_length = desc->lights->sun.shadow.length;

  // Setup point light
  shadow_pass_texture_create(&(ShadowPassTextureDescriptor){
      .dimension = WGPUTextureViewDimension_CubeArray, // Cube array
      .layer_count =
          MAX(point_light_length, LIGHT_MAX_CAPACITY) * LIGHT_POINT_VIEWS,
      .device = desc->device,
      .queue = desc->queue,
      .width = SHADOW_MAP_SIZE,
      .height = SHADOW_MAP_SIZE,
      .pass = &desc->lights->point.shadow.pass,
      .draw_list = desc->draw_list,
  });

  // Setup directional lights
  shadow_pass_texture_create(&(ShadowPassTextureDescriptor){
      .dimension = WGPUTextureViewDimension_2DArray, // 2D Array
      .layer_count =
          MAX(spot_light_length + sun_light_length, LIGHT_MAX_CAPACITY),
      .device = desc->device,
      .queue = desc->queue,
      .width = SHADOW_MAP_SIZE,
      .height = SHADOW_MAP_SIZE,
      .pass = &desc->lights->spot.shadow.pass,
      .draw_list = desc->draw_list,
  });


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
  WGPUTexture color_texture =
      wgpuDeviceCreateTexture(desc->device, &texture_descriptor_color);
  WGPUTextureView color_view = wgpuTextureCreateView(
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

  // Setup light depth texture

  // Create depth texture
  WGPUTexture depth_texture =
      wgpuDeviceCreateTexture(desc->device, &texture_descriptor_depth);
  WGPUTextureView depth_view = wgpuTextureCreateView(
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

  // set render pass
  render_pass_create(desc->pass,
                     &(RenderPassCreateDescriptor){
                         .label = "Shadow Map Pass",
                         .color =
                             &(RenderPassColorAttachment){
                                 .texture = color_texture,
                                 .view = color_view,
                                 .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
                                 .load_op = WGPULoadOp_Clear,
                                 .store_op = WGPUStoreOp_Store,
                                 .depth_slice = WGPU_DEPTH_SLICE_UNDEFINED,
                             },
                         .depth =
                             &(RenderPassDepthAttachment){
                                 .texture = depth_texture,
                                 .view = depth_view,
                                 .clear_value = 1.0f,
                                 .load_op = WGPULoadOp_Clear,
                                 .store_op = WGPUStoreOp_Store,
                             },
                         .device = desc->device,
                         .queue = desc->queue,
                         .height = desc->height,
                         .width = desc->width,
                         .swapchain = NULL,
                         .multisample = PipelineMultisampleCount_1x,
                         .draw_list = desc->draw_list,
                     });
}

void shadow_map_pass_preprocessor_callback(const RenderPass *pass, Mesh *mesh,
                                           void *userData) {

  LightShadowData *data = (LightShadowData *)userData;

  // swap pipeline (cull front/back)
  mesh_shader(mesh, MeshShader_Shadow)->pipeline = data->pipeline;

  // update each mesh shadow uniforms with current light view
  shader_update_bind_group_offset(mesh_shader(mesh, MeshShader_Shadow), 0, 0,
                                  data->view_offset);

}
