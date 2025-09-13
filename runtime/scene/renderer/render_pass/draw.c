#include "draw.h"
#include "../runtime/mesh/shader/shader.h"
#include "webgpu/webgpu.h"

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
void render_pass_command_draw(RenderPass *pass,
                              const RenderPassDrawOptions *overrides) {

  WGPUTextureView src_color_view = pass->color.attachment.view;
  WGPUTextureView src_depth_view = pass->depth.attachment.view;

  if (overrides && overrides->color)
    pass->color.attachment.view = overrides->color;

  if (overrides && overrides->depth)
    pass->depth.attachment.view = overrides->depth;

  {
    WGPURenderPassEncoder pass_encoder = wgpuCommandEncoderBeginRenderPass(
        pass->command_encoder,
        &(WGPURenderPassDescriptor){
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

  pass->color.attachment.view = src_color_view;
  pass->depth.attachment.view = src_depth_view;
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

static int t = 0;
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
  for (size_t i = 0; i < list->length; i++) {
    list->passes[i].command_encoder = render_encoder;
    // render each passes to commmon msaa texture
    list->passes[i].color.attachment.view = list->resolve_view;
    render_pass_command_draw(&list->passes[i], RENDER_PASS_VIEW_OVERRIDE_NONE);
  }

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
                                  .storeOp = WGPUStoreOp_Discard,
                              },
                      });
  wgpuRenderPassEncoderEnd(resolve_pass);

  render_pass_list_command_end(list, render_encoder);

  // finally release swapchain texture
  wgpuTextureViewRelease(swapchain_view);
  wgpuRenderPassEncoderRelease(resolve_pass);
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
    list->passes[i].command_encoder = render_encoder;
    // render each passes directly to swapchain
    list->passes[i].color.attachment.view = swapchain_view;
    render_pass_command_draw(&list->passes[i], RENDER_PASS_VIEW_OVERRIDE_NONE);
  }

  render_pass_list_command_end(list, render_encoder);

  // finally release swapchain texture
  wgpuTextureViewRelease(swapchain_view);
}

void render_pass_list_draw_offscreen(RenderPassList *list) {

  WGPUCommandEncoder render_encoder =
      wgpuDeviceCreateCommandEncoder(list->device, NULL);

  for (size_t i = 0; i < list->length; i++)
    render_pass_command_draw(&list->passes[i], RENDER_PASS_VIEW_OVERRIDE_NONE);

  render_pass_list_command_end(list, render_encoder);
}

/**
   Look up pass color draw based on msaa type

   Depending on mono sampling or multi sampling, the pass color attachment of
   renderer will be different (i.e. no resolveTarget for monosampling). As to
   avoid branching during the draw function, we set those callback as parameters
   before calling the draw.

   MSAA Texture (Nx) ===> resolved ===> Swapchain texture (1x)
 */

void render_pass_draw_onscreen_multisample(
    RenderPass *pass, const RenderPassDrawOptions *overrides) {

  pass->command_encoder = wgpuDeviceCreateCommandEncoder(pass->device, NULL);

  // get swapchain view to be resolved
  WGPUTextureView swapchain_view =
      wgpuSwapChainGetCurrentTextureView(pass->swapchain);

  render_pass_command_draw(pass, overrides);
  render_pass_command_end(pass);
}

void render_pass_draw_onscreen_monosample(
    RenderPass *pass, const RenderPassDrawOptions *overrides) {

  pass->command_encoder = wgpuDeviceCreateCommandEncoder(pass->device, NULL);

  // get swapchain view to be resolved
  WGPUTextureView swapchain_view =
      wgpuSwapChainGetCurrentTextureView(pass->swapchain);

  pass->color.attachment.view = swapchain_view;

  render_pass_command_draw(pass, overrides);
  render_pass_command_end(pass);
}

void render_pass_draw_offscreen(RenderPass *pass,
                                const RenderPassDrawOptions *overrides) {

  pass->command_encoder = wgpuDeviceCreateCommandEncoder(pass->device, NULL);

  render_pass_command_draw(pass, overrides);
  render_pass_command_end(pass);
}

void render_pass_draw(RenderPass *pass,
                      const RenderPassDrawOptions *overrides) {

  pass->draw_callback(pass, overrides);
}

WGPUCommandEncoder render_pass_command_begin(RenderPass *pass) {
  pass->command_encoder = wgpuDeviceCreateCommandEncoder(pass->device, NULL);
  return pass->command_encoder;
}

void render_pass_command_end(RenderPass *pass) {

  // create command buffer
  WGPUCommandBuffer render_buffer = wgpuCommandEncoderFinish(
      pass->command_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(pass->queue, 1, &render_buffer);

  // release command encoder
  wgpuCommandEncoderRelease(pass->command_encoder);
  pass->command_encoder = NULL;

  wgpuCommandBufferRelease(render_buffer);
}

void render_pass_list_command_end(RenderPassList *list,
                                  WGPUCommandEncoder command_encoder) {

  // create command buffer
  WGPUCommandBuffer render_buffer = wgpuCommandEncoderFinish(
      command_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(list->queue, 1, &render_buffer);

  // release command encoder
  wgpuCommandEncoderRelease(command_encoder);

  wgpuCommandBufferRelease(render_buffer);
}
