#include "core.h"
#include "../runtime/mesh/shader/shader.h"

static inline void
probe_reflection_list_create_texture(const ProbeReflectionTextureDescriptor *);

void probe_reflection_list_create_texture(
    const ProbeReflectionTextureDescriptor *desc) {

  /*
    === COLOR ===
   */

  *desc->color = wgpuDeviceCreateTexture(
      desc->device, &(WGPUTextureDescriptor){
                        .label = "Probe Reflection List Texture Color",
                        .size =
                            (WGPUExtent3D){
                                .width = desc->resolution,
                                .height = desc->resolution,
                                .depthOrArrayLayers = desc->layer_count,
                            },
                        .format = TEXTURE_FORMAT_ONSCREEN_DEFAULT,
                        .usage = WGPUTextureUsage_CopyDst |
                                 WGPUTextureUsage_RenderAttachment |
                                 WGPUTextureUsage_TextureBinding,
                        .dimension = WGPUTextureDimension_2D,
                        .mipLevelCount = 1,
                        .sampleCount = 1,
                    });

  *desc->color_view = wgpuTextureCreateView(
      *desc->color, &(WGPUTextureViewDescriptor){
                        .label = "Probe Reflection List View Color",
                        .format = TEXTURE_FORMAT_ONSCREEN_DEFAULT,
                        .dimension = desc->view_dimension,
                        .baseMipLevel = 0,
                        .mipLevelCount = 1,
                        .baseArrayLayer = 0,
                        .arrayLayerCount = desc->layer_count,
                        .aspect = WGPUTextureAspect_Undefined,
                    });

  /*
    === DEPTH ===
   */

  *desc->depth = wgpuDeviceCreateTexture(
      desc->device, &(WGPUTextureDescriptor){
                        .label = "Probe Reflection List Texture Depth",
                        .size =
                            (WGPUExtent3D){
                                .width = desc->resolution,
                                .height = desc->resolution,
                                .depthOrArrayLayers = desc->layer_count,
                            },
                        .format = WGPUTextureFormat_Depth24Plus,
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
                        .format = WGPUTextureFormat_Depth24Plus,
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
        .device = desc->device,
        .view_dimension = desc->render_pass->view_dimension,
        .layer_count = desc->render_pass->layer_count,
    });

    // create render pass preset
    render_pass_create(desc->render_pass->handle,
                       &(RenderPassCreateDescriptor){
                           .label = "Probe Reflection List Render Pass",
                           .device = desc->device,
                           .queue = desc->queue,
                           .height = desc->render_pass->resolution,
                           .width = desc->render_pass->resolution,
                           .draw_list = desc->render_pass->draw_list,
                           .multisample = PipelineMultisampleCount_1x,
                           .swapchain = NULL,
                           .color =
                               &(RenderPassColorAttachment){
                                   .clear_value = {0},
                                   .depth_slice = WGPU_DEPTH_SLICE_UNDEFINED,
                                   .load_op = WGPULoadOp_Clear,
                                   .store_op = WGPUStoreOp_Store,
                                   .texture = color_texture,
                                   .view = color_view,
                               },
                           .depth =
                               &(RenderPassDepthAttachment){
                                   .clear_value = 1.0f,
                                   .load_op = WGPULoadOp_Clear,
                                   .store_op = WGPUStoreOp_Store,
                                   .read_only = false,
                                   .texture = depth_texture,
                                   .view = depth_view,
                               },
                       });
  }

  return create;
}

void probe_reflection_list_draw_preprocessor(const RenderPass *pass, Mesh *mesh,
                                             void *data) {

  ProbeReflectionListPreprocessorData *cast_data =
      (ProbeReflectionListPreprocessorData *)data;


  Shader *shader = mesh_shader(mesh, MeshShader_Reflection);  
  shader_update_bind_group_offset(shader, 0, 1, cast_data->camera_offset);
}
