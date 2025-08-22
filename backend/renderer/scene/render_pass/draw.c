#include "draw.h"

/**



  ▗▄▄▄ ▗▄▄▖  ▗▄▖ ▗▖ ▗▖
  ▐▌  █▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌
  ▐▌  █▐▛▀▚▖▐▛▀▜▌▐▌ ▐▌
  ▐▙▄▄▀▐▌ ▐▌▐▌ ▐▌▐▙█▟▌





 */

/**
   Traverse a specific pass draw list and draw meshes onto the given render pass
   encoder
 */
void render_pass_draw_pass(RenderPass *pass,
                           WGPUCommandEncoder render_encoder) {

  pass->encoder = wgpuCommandEncoderBeginRenderPass(
      render_encoder, &(WGPURenderPassDescriptor){
                          .label = pass->label,
                          .colorAttachmentCount = 1,
                          .colorAttachments = &pass->color.attachment,
                          .depthStencilAttachment = &pass->depth.attachment,
                      });

  // Go through and draw each mode render pass
  // Draw meshes
  // loop through mesh lists and draw meshes
  for (size_t j = 0; j < pass->draw_list.length; j++) {

    // retrieve layout
    RenderPassDrawLayout *list = &pass->draw_list.entries[j];
    mesh_get_topology_callback target_topology = list->topology_callback;
    mesh_get_shader_callback target_shader = list->shader_callback;
    render_pass_mesh_preprocessor_callback mesh_preprocessor =
        list->mesh_preprocessor_callback;
    MeshRefList *meshes = list->meshes;

    // draw mesh with layout callbacks
    for (size_t k = 0; k < meshes->length; k++) {
      Mesh *mesh = meshes->entries[k];

      if (mesh_preprocessor)
        mesh_preprocessor(pass, mesh, list->mesh_preprocessor_data);

      mesh_draw(target_topology(mesh), target_shader(mesh), pass->encoder);
    }
  }

  wgpuRenderPassEncoderEnd(pass->encoder);
}

/**
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
void render_pass_list_draw_onscreen_multisample(RenderPassList *list) {

  /*
    Create 1 command encoder for all the render passes.
    Encoder records GPU operations such as:
    - Texture upload
    - Buffer upload
    - Render passes
    - Compute passes
   */

  WGPUCommandEncoder render_encoder =
      wgpuDeviceCreateCommandEncoder(list->device, NULL);

  // get swapchain view to be resolved
  WGPUTextureView swapchain_view =
      wgpuSwapChainGetCurrentTextureView(list->swapchain);

  // Go through and draw each mode render pass
  for (size_t i = 0; i < list->length; i++)
    render_pass_draw_pass(&list->passes[i], render_encoder);

  // resolve pass (MSAA only)
  WGPURenderPassEncoder resolve_pass = wgpuCommandEncoderBeginRenderPass(
      render_encoder, &(WGPURenderPassDescriptor){
                          .label = "MSAA Resolve Pass",
                          .colorAttachmentCount = 1,
                          .colorAttachments =
                              &(WGPURenderPassColorAttachment){
                                  .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
                                  // pass last pass 4x sample as view
                                  .view = list->resolve_view,
                                  .resolveTarget = swapchain_view, // 1x sampled
                                  .loadOp = WGPULoadOp_Load,
                                  .storeOp = WGPUStoreOp_Store,
                              },
                      });
  wgpuRenderPassEncoderEnd(resolve_pass);

  // create command buffer
  WGPUCommandBuffer render_buffer =
      wgpuCommandEncoderFinish(render_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(list->queue, 1, &render_buffer);

  // release all passes encoders
  for (size_t i = 0; i < list->length; i++)
    wgpuRenderPassEncoderRelease(list->passes[i].encoder);

  // release command encoder
  wgpuCommandEncoderRelease(render_encoder);

  wgpuCommandBufferRelease(render_buffer);

  // finally release swapchain texture
  wgpuTextureViewRelease(swapchain_view);
}

void render_pass_list_draw_onscreen_monosample(RenderPassList *list) {

  /*
    Create 1 command encoder for all the render passes.
    Encoder records GPU operations such as:
    - Texture upload
    - Buffer upload
    - Render passes
    - Compute passes
   */

  WGPUCommandEncoder render_encoder =
      wgpuDeviceCreateCommandEncoder(list->device, NULL);

  // get swapchain view to be resolved
  WGPUTextureView swapchain_view =
      wgpuSwapChainGetCurrentTextureView(list->swapchain);

  // Go through and draw each mode render pass
  for (size_t i = 0; i < list->length; i++) {
    list->passes[i].color.attachment.view = swapchain_view;
    render_pass_draw_pass(&list->passes[i], render_encoder);
  }

  // create command buffer
  WGPUCommandBuffer render_buffer =
      wgpuCommandEncoderFinish(render_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(list->queue, 1, &render_buffer);

  // release all passes encoders
  for (size_t i = 0; i < list->length; i++)
    wgpuRenderPassEncoderRelease(list->passes[i].encoder);

  // release command encoder
  wgpuCommandEncoderRelease(render_encoder);

  wgpuCommandBufferRelease(render_buffer);

  // finally release swapchain texture
  wgpuTextureViewRelease(swapchain_view);
}

void render_pass_list_draw_offscreen(RenderPassList *list) {

  WGPUCommandEncoder render_encoder =
      wgpuDeviceCreateCommandEncoder(list->device, NULL);

  for (size_t i = 0; i < list->length; i++)
    render_pass_draw_pass(&list->passes[i], render_encoder);

  // create command buffer
  WGPUCommandBuffer render_buffer =
      wgpuCommandEncoderFinish(render_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(list->queue, 1, &render_buffer);

  // release all passes encoders
  for (size_t i = 0; i < list->length; i++)
    wgpuRenderPassEncoderRelease(list->passes[i].encoder);

  // release command encoder
  wgpuCommandEncoderRelease(render_encoder);

  wgpuCommandBufferRelease(render_buffer);
}

/**
   Look up pass color draw based on msaa type

   Depending on mono sampling or multi sampling, the pass color attachment of
   renderer will be different (i.e. no resolveTarget for monosampling). As to
   avoid branching during the draw function, we set those callback as parameters
   before calling the draw.

   MSAA Texture (Nx) ===> resolved ===> Swapchain texture (1x)
 */

void render_pass_draw_onscreen_multisample(RenderPass *pass) {

  WGPUCommandEncoder render_encoder =
      wgpuDeviceCreateCommandEncoder(pass->device, NULL);

  // get swapchain view to be resolved
  WGPUTextureView swapchain_view =
      wgpuSwapChainGetCurrentTextureView(pass->swapchain);

  render_pass_draw_pass(pass, render_encoder);

  // create command buffer
  WGPUCommandBuffer render_buffer =
      wgpuCommandEncoderFinish(render_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(pass->queue, 1, &render_buffer);

  // release all passes encoders
  wgpuRenderPassEncoderRelease(pass->encoder);

  // release command encoder
  wgpuCommandEncoderRelease(render_encoder);

  wgpuCommandBufferRelease(render_buffer);

  // finally release swapchain texture
  wgpuTextureViewRelease(swapchain_view);
}

void render_pass_draw_onscreen_monosample(RenderPass *pass) {

  WGPUCommandEncoder render_encoder =
      wgpuDeviceCreateCommandEncoder(pass->device, NULL);

  // get swapchain view to be resolved
  WGPUTextureView swapchain_view =
      wgpuSwapChainGetCurrentTextureView(pass->swapchain);

  pass->color.attachment.view = swapchain_view;
  render_pass_draw_pass(pass, render_encoder);

  // create command buffer
  WGPUCommandBuffer render_buffer =
      wgpuCommandEncoderFinish(render_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(pass->queue, 1, &render_buffer);

  // release all passes encoders
  wgpuRenderPassEncoderRelease(pass->encoder);

  // release command encoder
  wgpuCommandEncoderRelease(render_encoder);

  wgpuCommandBufferRelease(render_buffer);

  // finally release swapchain texture
  wgpuTextureViewRelease(swapchain_view);
}

void render_pass_draw_offscreen(RenderPass *pass) {

  WGPUCommandEncoder render_encoder =
      wgpuDeviceCreateCommandEncoder(pass->device, NULL);

  render_pass_draw_pass(pass, render_encoder);

  // create command buffer
  WGPUCommandBuffer render_buffer =
      wgpuCommandEncoderFinish(render_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(pass->queue, 1, &render_buffer);

  // release all passes encoders
  wgpuRenderPassEncoderRelease(pass->encoder);

  // release command encoder
  wgpuCommandEncoderRelease(render_encoder);

  wgpuCommandBufferRelease(render_buffer);
}

void render_pass_draw(RenderPass *pass,
                      const RenderPassViewOverride *overrides) {

  WGPUTextureView src_color_view = pass->color.attachment.view;
  WGPUTextureView src_depth_view = pass->depth.attachment.view;

  if (overrides->color)
    pass->color.attachment.view = overrides->color;

  if (overrides->depth)
    pass->depth.attachment.view = overrides->depth;

  pass->draw_callback(pass);

  if (overrides->color)
    wgpuTextureViewRelease(pass->color.attachment.view);

  if (overrides->depth)
    wgpuTextureViewRelease(pass->depth.attachment.view);

  // put back the original views
  pass->color.attachment.view = src_color_view;
  pass->depth.attachment.view = src_depth_view;
}
