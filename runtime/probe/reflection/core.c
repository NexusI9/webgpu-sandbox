#include "core.h"

#include <stdbool.h>
#include <webgpu/webgpu.h>

#include "backend/context.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/pipeline/render.h"
#include "runtime/scene/renderer/render_pass/core.h"
#include "runtime/shader/core.h"
#include "runtime/shader/update.h"
#include "runtime/texture/core.h"
#include "utils/dyli.h"

static inline void
probe_reflection_list_create_texture(const ProbeReflectionTextureDescriptor *);

void probe_reflection_list_create_texture(
    const ProbeReflectionTextureDescriptor *desc) {

  /*
    === COLOR ===
   */

  *desc->color = wgpuDeviceCreateTexture(
      context_device(),
      &(WGPUTextureDescriptor){
          .label = "Probe Reflection List Texture Color",
          .size =
              (WGPUExtent3D){
                  .width = desc->resolution,
                  .height = desc->resolution,
                  .depthOrArrayLayers = desc->layer_count,
              },
          .format = TEXTURE_FORMAT_OFFSCREEN,
          .usage = WGPUTextureUsage_TextureBinding   // read texture in shader
                   | WGPUTextureUsage_StorageBinding // write texture in shader
                   | WGPUTextureUsage_CopyDst        // upload the input data
                   | WGPUTextureUsage_RenderAttachment,
          .dimension = WGPUTextureDimension_2D,
          .mipLevelCount = PROBE_REFLECTION_MIPMAP_COUNT,
          .sampleCount = 1,
      });

  *desc->color_view = wgpuTextureCreateView(
      *desc->color, &(WGPUTextureViewDescriptor){
                        .label = "Probe Reflection List View Color",
                        .format = wgpuTextureGetFormat(*desc->color),
                        .dimension = desc->view_dimension,
                        .baseMipLevel = PROBE_REFLECTION_MIPMAP_COUNT -
                                        1, // Use level 1 for blurriness
                        .mipLevelCount = 1,
                        .baseArrayLayer = 0,
                        .arrayLayerCount = desc->layer_count,
                        .aspect = WGPUTextureAspect_Undefined,
                    });

  /*
    === DEPTH ===
   */

  *desc->depth = wgpuDeviceCreateTexture(
      context_device(), &(WGPUTextureDescriptor){
                            .label = "Probe Reflection List Texture Depth",
                            .size =
                                (WGPUExtent3D){
                                    .width = desc->resolution,
                                    .height = desc->resolution,
                                    .depthOrArrayLayers = desc->layer_count,
                                },
                            .format = TEXTURE_FORMAT_DEPTH,
                            .usage = WGPUTextureUsage_CopyDst |
                                     WGPUTextureUsage_RenderAttachment |
                                     WGPUTextureUsage_TextureBinding,
                            .dimension = WGPUTextureDimension_2D,
                            .mipLevelCount = 1,
                            .sampleCount = 1,
                        });

  *desc->depth_view = wgpuTextureCreateView(
      *desc->depth, &(WGPUTextureViewDescriptor){
                        .label = "Probe Reflection List View Depth",
                        .dimension = desc->view_dimension,
                        .format = wgpuTextureGetFormat(*desc->depth),
                        .baseMipLevel = 0,
                        .mipLevelCount = 1,
                        .baseArrayLayer = 0,
                        .arrayLayerCount = desc->layer_count,
                        .aspect = WGPUTextureAspect_DepthOnly,
                    });
}

DynamicListStatus
probe_reflection_list_create_core(const ProbeReflectionCreateCore *desc) {

  DynamicListStatus create =
      dyli_create((void *)desc->probe_list->entries, desc->probe_list->capacity,
                  desc->probe_list->length, desc->probe_list->type_size,
                  desc->probe_list->num, desc->probe_list->label);

  if (create == DynamicListStatus_Success) {

    // create list textures array
    WGPUTexture color_texture, depth_texture;
    WGPUTextureView color_view, depth_view;

    probe_reflection_list_create_texture(&(ProbeReflectionTextureDescriptor){
        .color = &color_texture,
        .depth = &depth_texture,
        .color_view = &color_view,
        .depth_view = &depth_view,
        .resolution = desc->render_pass->resolution,
        .view_dimension = desc->render_pass->view_dimension,
        .layer_count = desc->render_pass->layer_count,
    });

    // create render pass preset
    render_pass_create(
        desc->render_pass->handle,
        &(RenderPassCreateDescriptor){
            .type = RenderPassType_OffScreen,
            .label = "Probe Reflection List Render Pass",
            .height = desc->render_pass->resolution,
            .width = desc->render_pass->resolution,
            .draw_list = desc->render_pass->draw_list,
            .multisample = PipelineMultisampleCount_1x,
            .color =
                &(RenderPassColorAttachment){
                    .texture = color_texture,
                    .attachment =
                        {
                            .clearValue = {0},
                            .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
                            .loadOp = WGPULoadOp_Clear,
                            .storeOp = WGPUStoreOp_Store,
                            .view = color_view,
                        },
                },
            .depth =
                &(RenderPassDepthAttachment){
                    .texture = depth_texture,
                    .attachment =
                        {
                            .depthClearValue = 1.0f,
                            .depthLoadOp = WGPULoadOp_Clear,
                            .depthStoreOp = WGPUStoreOp_Store,
                            .depthReadOnly = false,
                            .view = depth_view,
                        },
                },
        });
  }

  return create;
}

void probe_reflection_list_update_resolution(
    RenderPass *pass, const TextureResolution resolution,
    const WGPUTextureViewDimension view_dimension) {

  if (resolution == wgpuTextureGetWidth(pass->color.texture) ||
      resolution == wgpuTextureGetHeight(pass->color.texture))
    return;

  uint32_t layer_count = wgpuTextureGetDepthOrArrayLayers(pass->color.texture);

  // === clean up ===
  {
    if (pass->color.texture)
      wgpuTextureRelease(pass->color.texture);

    if (pass->depth.texture)
      wgpuTextureRelease(pass->depth.texture);

    if (pass->color.views[0])
      wgpuTextureViewRelease(pass->color.views[0]);

    if (pass->depth.views[0])
      wgpuTextureViewRelease(pass->depth.views[0]);
  }

  probe_reflection_list_create_texture(&(ProbeReflectionTextureDescriptor){
      .color = &pass->color.texture,
      .color_view = &pass->color.views[0],
      .depth = &pass->depth.texture,
      .depth_view = &pass->depth.views[0],
      .view_dimension = view_dimension,
      .layer_count = layer_count,
      .resolution = resolution,
  });

  pass->color.attachment.view = pass->color.views[0];
  pass->depth.attachment.view = pass->depth.views[0];
}

void probe_reflection_list_draw_preprocessor(const RenderPass *pass, Mesh *mesh,
                                             void *data) {

  ProbeReflectionListPreprocessorData *cast_data =
      (ProbeReflectionListPreprocessorData *)data;

  Shader *shader = mesh_shader(mesh, MeshShader_Reflection);
  shader_update_bind_group_offset(shader, 0, 1, cast_data->camera_offset,
                                  ShaderUpdateFlag_None);
}
