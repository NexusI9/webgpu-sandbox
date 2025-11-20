#ifndef _GUI_DRAW_H_
#define _GUI_DRAW_H_

#include "backend/context.h"
#include "core.h"
#include "utils/defines.h"

EXTERN_C_BEGIN

static inline void gui_draw_begin(Gui *gui) {

  WGPUCommandEncoderDescriptor com_enc_desc = {.label = "Scene UI Command"};
  gui->command_encoder =
      wgpuDeviceCreateCommandEncoder(context_device(), &com_enc_desc);

  gui->swapchain_view = wgpuSwapChainGetCurrentTextureView(context_swapchain());

  WGPURenderPassColorAttachment color_attachment = {
      .view = gui->swapchain_view,
      .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
      .resolveTarget = NULL,
      .loadOp = WGPULoadOp_Load,
      .storeOp = WGPUStoreOp_Store,
      .clearValue = {0.0f, 0.0f, 0.0f, 1.0f},
  };

  WGPURenderPassDepthStencilAttachment depth_attachment = {
      .view = gui->depth_view,
      .depthLoadOp = WGPULoadOp_Clear,
      .depthStoreOp = WGPUStoreOp_Store,
      .depthClearValue = 1.0f,
      .depthReadOnly = false,
  };

  WGPURenderPassDescriptor render_pass_desc = {
      .colorAttachmentCount = 1,
      .colorAttachments = &color_attachment,
      .depthStencilAttachment = &depth_attachment,
  };

  gui->pass_encoder = wgpuCommandEncoderBeginRenderPass(gui->command_encoder,
                                                        &render_pass_desc);

}

static inline void gui_draw_end(Gui *gui) {
  wgpuRenderPassEncoderEnd(gui->pass_encoder);
  WGPUCommandBuffer command_buffer =
      wgpuCommandEncoderFinish(gui->command_encoder, NULL);
  wgpuQueueSubmit(context_queue(), 1, &command_buffer);
  wgpuTextureViewRelease(gui->swapchain_view);
}

EXTERN_C_END

#endif
