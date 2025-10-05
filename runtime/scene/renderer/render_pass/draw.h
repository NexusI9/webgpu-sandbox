#ifndef _RENDER_PASS_DRAW_H_
#define _RENDER_PASS_DRAW_H_

#include "core.h"
#include "webgpu/webgpu.h"

#include <stddef.h>

#include "backend/context.h"
#include "backend/postfx/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/scene/renderer/render_pass/core.h"
#include "webgpu/webgpu.h"

/**

  ▗▄▄▄ ▗▄▄▖  ▗▄▖ ▗▖ ▗▖
  ▐▌  █▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌
  ▐▌  █▐▛▀▚▖▐▛▀▜▌▐▌ ▐▌
  ▐▙▄▄▀▐▌ ▐▌▐▌ ▐▌▐▙█▟▌

   Used in hot path so we static inline the calls

 */

/**
   Traverse a specific pass draw list and draw meshes onto the given render pass
   encoder
 */

// Pass
static inline void render_pass_draw(RenderPass *);
static inline void render_pass_draw_callback_default(RenderPass *);
static inline void render_pass_draw_callback_resolve(RenderPass *);
static inline void render_pass_draw_callback_swapchain(RenderPass *);

// Pass Immediate mode functions
static inline WGPUCommandEncoder render_pass_im_begin(RenderPass *);
static inline void render_pass_im_draw(RenderPass *);
static inline void render_pass_im_end(RenderPass *);
static inline void render_pass_im_set_views(RenderPass *,
                                            const RenderPassDrawOptions *);

// Pass List
static inline void render_pass_list_draw(RenderPassList *);
static inline void list_tmp(RenderPassList *);

/**
   ▗▄▄▖  ▗▄▖  ▗▄▄▖ ▗▄▄▖    ▗▖   ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖
   ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌       ▐▌     █  ▐▌     █
   ▐▛▀▘ ▐▛▀▜▌ ▝▀▚▖ ▝▀▚▖    ▐▌     █   ▝▀▚▖  █
   ▐▌   ▐▌ ▐▌▗▄▄▞▘▗▄▄▞▘    ▐▙▄▄▖▗▄█▄▖▗▄▄▞▘  █

   Draw the meshes of a given list along with a target topology and shader.
   The function will be call with attributes coming from the renderer
   draw_callbacks array.

   Using topology and shader callbacks allow greater flexibility when it comes
   to the different display modes.

   For each multisample pass we render to the scene renderer shared color and
   depth textures (accumulate).
   And after all passes we make a final one (resolve pass) where we send the
   shared texture to the swapchain.

      Render Pass 1     Render Pass 2       Resolve Pass
      +-----------+     +-----------+      +-----------+
      |           |     |           |      |           |
      |  Texture  |  +  |  Texture  |  =>  | Swapchain |
      |           |     |           |      |           |
      +-----------+     +-----------+      +-----------+

  For monosample we directly render to the swapchain as it is 1x sampled

      Render Pass 1     Render Pass 2
      +-----------+     +-----------+
      |           |     |           |
      | Swapchain |  +  | Swapchain |
      |           |     |           |
      +-----------+     +-----------+

 */

void render_pass_list_draw(RenderPassList *list) {

  /*
    Create 1 command encoder for all the render passes.
    Encoder records GPU operations such as:
    - Texture upload
    - Buffer upload
    - Render passes
    - Compute passes
   */

  WGPUCommandEncoder command_encoder =
      wgpuDeviceCreateCommandEncoder(context_device(), NULL);

  // get swapchain view to be resolved
  WGPUTextureView swapchain_view =
      wgpuSwapChainGetCurrentTextureView(context_swapchain());

  // Go through and draw each mode render pass
  for (size_t i = 0; i < list->length; i++) {
    RenderPass *pass = &list->passes[i];
    pass->command_encoder = command_encoder;
    render_pass_draw(pass);
  }

  {
    // create command buffer
    WGPUCommandBuffer render_buffer =
        wgpuCommandEncoderFinish(command_encoder, NULL);
    wgpuQueueSubmit(context_queue(), 1, &render_buffer);

    // release command encoder
    wgpuCommandEncoderRelease(command_encoder);
    wgpuCommandBufferRelease(render_buffer);
    wgpuTextureViewRelease(swapchain_view);
  }
}

/**
    ▗▄▄▖ ▗▄▖ ▗▖   ▗▖   ▗▄▄▖  ▗▄▖  ▗▄▄▖▗▖ ▗▖ ▗▄▄▖
   ▐▌   ▐▌ ▐▌▐▌   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌▗▞▘▐▌
   ▐▌   ▐▛▀▜▌▐▌   ▐▌   ▐▛▀▚▖▐▛▀▜▌▐▌   ▐▛▚▖  ▝▀▚▖
   ▝▚▄▄▖▐▌ ▐▌▐▙▄▄▖▐▙▄▄▖▐▙▄▞▘▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌▗▄▄▞▘

   Look up pass color draw based on msaa type

   Depending on mono sampling or multi sampling, the pass color attachment of
   renderer will be different (i.e. no resolveTarget for monosampling). As to
   a  void branching during the draw function, we set those callback as
   parameters before calling the draw.

   MSAA Texture (Nx) ===> resolved ===> Swapchain texture (1x)
 */

void render_pass_draw_callback_default(RenderPass *pass) {
  render_pass_im_draw(pass);
}

/*
  Only for On Screen last render pass in a pass list
  Resolve msaa => revolve view => blit => swapchain
 */
void render_pass_draw_callback_resolve(RenderPass *pass) {

  // Transfert cached resolve view (sampled 1x) to pass resolve
  pass->color.attachment.resolveTarget = pass->color.resolve_view;
  render_pass_im_draw(pass);

  // Blit pass
  {
    WGPURenderPassColorAttachment color_attachment = {
        .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
        .view = wgpuSwapChainGetCurrentTextureView(context_swapchain()),
        .loadOp = WGPULoadOp_Load,
        .storeOp = WGPUStoreOp_Store,
    };

    WGPURenderPassDescriptor pass_desc = {
        .label = "Resolve Pass",
        .colorAttachmentCount = 1,
        .colorAttachments = &color_attachment,
    };

    // RESOLVE (1x) ==> POSTFX ==> SWAPCHAIN
    WGPURenderPassEncoder resolve_pass =
        wgpuCommandEncoderBeginRenderPass(pass->command_encoder, &pass_desc);
    {
      // POST FX
      post_fx_blit(&pass->post_fx, pass->color.attachment.resolveTarget,
                   resolve_pass);
    }
    wgpuRenderPassEncoderEnd(resolve_pass);
  }
}

// resolve to swapchain
void render_pass_draw_callback_swapchain(RenderPass *pass) {
  pass->color.attachment.view =
      wgpuSwapChainGetCurrentTextureView(context_swapchain());
  render_pass_im_draw(pass);
}

void render_pass_draw(RenderPass *pass) { pass->draw_callback(pass); }

/*
   ▗▄▄▄▖▗▖  ▗▖▗▖  ▗▖▗▄▄▄▖▗▄▄▄ ▗▄▄▄▖ ▗▄▖▗▄▄▄▖▗▄▄▄▖
     █  ▐▛▚▞▜▌▐▛▚▞▜▌▐▌   ▐▌  █  █  ▐▌ ▐▌ █  ▐▌
     █  ▐▌  ▐▌▐▌  ▐▌▐▛▀▀▘▐▌  █  █  ▐▛▀▜▌ █  ▐▛▀▀▘
   ▗▄█▄▖▐▌  ▐▌▐▌  ▐▌▐▙▄▄▖▐▙▄▄▀▗▄█▄▖▐▌ ▐▌ █  ▐▙▄▄▖

               ▗▖  ▗▖ ▗▄▖ ▗▄▄▄ ▗▄▄▄▖
               ▐▛▚▞▜▌▐▌ ▐▌▐▌  █▐▌
               ▐▌  ▐▌▐▌ ▐▌▐▌  █▐▛▀▀▘
               ▐▌  ▐▌▝▚▄▞▘▐▙▄▄▀▐▙▄▄▖


 */

WGPUCommandEncoder render_pass_im_begin(RenderPass *pass) {
  pass->command_encoder =
      wgpuDeviceCreateCommandEncoder(context_device(), NULL);
  return pass->command_encoder;
}

void render_pass_im_set_views(RenderPass *pass,
                              const RenderPassDrawOptions *overrides) {
  if (overrides && overrides->color)
    pass->color.attachment.view = overrides->color;

  if (overrides && overrides->depth)
    pass->depth.attachment.view = overrides->depth;
}

void render_pass_im_end(RenderPass *pass) {

  // create command buffer
  WGPUCommandBuffer render_buffer = wgpuCommandEncoderFinish(
      pass->command_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(context_queue(), 1, &render_buffer);

  // release command encoder
  wgpuCommandEncoderRelease(pass->command_encoder);
  pass->command_encoder = NULL;

  wgpuCommandBufferRelease(render_buffer);
}

void render_pass_im_draw(RenderPass *pass) {

  const WGPURenderPassDescriptor pass_desc = {
      .label = pass->label,
      .colorAttachmentCount = 1,
      .colorAttachments = &pass->color.attachment,
      .depthStencilAttachment = &pass->depth.attachment,
  };

  WGPURenderPassEncoder pass_encoder =
      wgpuCommandEncoderBeginRenderPass(pass->command_encoder, &pass_desc);

  // Go through and draw each mode render pass
  // Draw meshes
  // loop through mesh lists and draw meshes
  for (size_t j = 0; j < pass->draw_list.length; j++) {

    // retrieve layout
    RenderPassDrawLayout *list = &pass->draw_list.entries[j];
    mesh_get_topology_callback target_topology = list->topology_callback;
    MeshShader target_shader = list->shader;
    render_pass_mesh_preprocessor_callback mesh_preprocessor =
        list->mesh_preprocessor_callback;
    MeshRefList *meshes = &list->drawn_meshes;

    // draw mesh with layout callbacks
    for (size_t k = 0; k < meshes->length; k++) {
      Mesh *mesh = meshes->entries[k];

      if (mesh_preprocessor)
        mesh_preprocessor(pass, mesh, list->mesh_preprocessor_data);

      mesh_draw(target_topology(mesh), mesh_shader(mesh, target_shader),
                pass_encoder);
    }
  }
  wgpuRenderPassEncoderEnd(pass_encoder);
  wgpuRenderPassEncoderRelease(pass_encoder);
}

#endif
