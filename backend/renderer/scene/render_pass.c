#include "render_pass.h"
#include "webgpu/webgpu.h"
#include <string.h>

static void render_pass_create_color_multisample(RenderPass *);
static void render_pass_create_color_monosample(RenderPass *);

static void render_pass_draw_layout(RenderPassLayout *,
                                    WGPURenderPassEncoder *);

static void render_pass_draw_monosample(RenderPass[RENDER_PASS_COUNT],
                                        RenderPassLayout *, WGPUTextureView *,
                                        WGPUTextureView *, WGPUTextureView *,
                                        WGPUCommandEncoder *);

static void render_pass_draw_multisample(RenderPass[RENDER_PASS_COUNT],
                                         RenderPassLayout *, WGPUTextureView *,
                                         WGPUTextureView *, WGPUTextureView *,
                                         WGPUCommandEncoder *);

/**
   Look up pass color draw based on msaa type

   Depending on mono sampling or multi sampling, the pass color attachment of
   renderer will be different (i.e. no resolveTarget for monosampling). As to
   avoid branching during the draw function, we set those callback as parameters
   before calling the draw.

   MSAA Texture (Nx) ===> resolved ===> Swapchain texture (1x)
 */
static const render_pass_draw_callback draw_callback[] = {
    [PipelineMultisampleCount_1x] = render_pass_draw_monosample,
    [PipelineMultisampleCount_4x] = render_pass_draw_multisample,
};

void render_pass_create(RenderPass *render_pass,
                        const RenderPassCreateDescriptor *desc) {

  // assign core attributes
  render_pass->label = strdup(desc->label);

  // assign color attributes
  render_pass->color.multisample = desc->color.multisample;

  render_pass->color.attachment = (WGPURenderPassColorAttachment){
      .clearValue = desc->color.clear_value,
      .depthSlice = desc->color.depth_slice,
      .loadOp = desc->color.load_op,
      .storeOp = desc->color.store_op,
  };

  // assign depth
  render_pass->depth.attachment = (WGPURenderPassDepthStencilAttachment){
      .depthClearValue = desc->depth.clear_value,
      .depthReadOnly = desc->depth.read_only,
      .depthLoadOp = desc->depth.load_op,
      .depthStoreOp = desc->depth.store_op,
  };
}

void render_pass_draw(RenderPassDrawDescriptor *desc) {

  /*
  Create 1 command encoder for all the render passes.
  Encoder records GPU operations such as:
  - Texture upload
  - Buffer upload
  - Render passes
  - Compute passes
*/
  WGPUCommandEncoder render_encoder =
      wgpuDeviceCreateCommandEncoder(*desc->device, NULL);

  // get swapchain view to be resolved
  WGPUTextureView swapchain_view =
      wgpuSwapChainGetCurrentTextureView(*desc->swapchain);

  draw_callback[desc->multisample](desc->pass_list, desc->pass_layout,
                                   desc->color_target, desc->depth_target,
                                   &swapchain_view, &render_encoder);

  // create command buffer
  WGPUCommandBuffer render_buffer =
      wgpuCommandEncoderFinish(render_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(*desc->queue, 1, &render_buffer);

  // release all passes encoders
  for (size_t i = 0; i < desc->pass_layout->length; i++) {
    const RenderPassType type = desc->pass_layout->entries[i].pass;
    const RenderPass *pass = &desc->pass_list[type];
    wgpuRenderPassEncoderRelease(pass->encoder);
  }

  // release command encoder
  wgpuCommandEncoderRelease(render_encoder);

  wgpuCommandBufferRelease(render_buffer);

  // finally release swapchain texture
  wgpuTextureViewRelease(swapchain_view);
}

/**
   Traverse a specific layout array and draw meshes onto the given render pass
   encoder
 */
static void render_pass_draw_layout(RenderPassLayout *pass_layout,
                                    WGPURenderPassEncoder *encoder) {

  // Draw meshes
  for (size_t i = 0; i < pass_layout->length; i++) {

    // retrieve each render pass entries (mesh/topo/shader)
    RenderPassDrawList *layout_list = &pass_layout->entries[i];

    // loop through mesh lists and draw meshes
    for (size_t j = 0; j < layout_list->length; j++) {

      // retrieve layout
      RenderPassDrawLayout *layout = &layout_list->entries[j];
      mesh_get_topology_callback target_topology = layout->topology_callback;
      mesh_get_shader_callback target_shader = layout->shader_callback;
      MeshRefList *meshes = layout->meshes;

      // draw mesh with layout callbacks
      for (size_t k = 0; k < meshes->length; k++) {
        Mesh *mesh = meshes->entries[k];
        mesh_draw(target_topology(mesh), target_shader(mesh), encoder);
      }
    }
  }
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
void render_pass_draw_multisample(RenderPass pass_list[RENDER_PASS_COUNT],
                                  RenderPassLayout *pass_layout,
                                  WGPUTextureView *shared_color_view,
                                  WGPUTextureView *shared_depth_view,
                                  WGPUTextureView *swapchain_view,
                                  WGPUCommandEncoder *encoder) {

  // Travese passes (Scene >> Gizmo >> ...)
  for (size_t i = 0; i < pass_layout->length; i++) {

    const RenderPassType type = pass_layout->entries[i].pass;
    // look-up renderer pass from the list depending on the type
    RenderPass *pass = &pass_list[type];

    // assign shared textures view amongst passes
    pass->color.attachment.view = *shared_color_view;
    pass->depth.attachment.view = *shared_depth_view;

    // begin render pass
    pass->encoder = wgpuCommandEncoderBeginRenderPass(
        *encoder, &(WGPURenderPassDescriptor){
                      .label = pass->label,
                      .colorAttachmentCount = 1,
                      .colorAttachments = &pass->color.attachment,
                      .depthStencilAttachment = &pass->depth.attachment,
                  });

    // draw layout (mesh > topo > shader)
    render_pass_draw_layout(pass_layout, &pass->encoder);

    // end render pass
    wgpuRenderPassEncoderEnd(pass->encoder);
  }

  // resolve pass (MSAA only)
  WGPURenderPassEncoder resolve_pass = wgpuCommandEncoderBeginRenderPass(
      *encoder,
      &(WGPURenderPassDescriptor){
          .label = "MSAA Resolve Pass",
          .colorAttachmentCount = 1,
          .colorAttachments =
              &(WGPURenderPassColorAttachment){
                  .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
                  .view = *shared_color_view,       // pass 4x sample as view
                  .resolveTarget = *swapchain_view, // 1x sampled
                  .loadOp = WGPULoadOp_Load,
                  .storeOp = WGPUStoreOp_Store,
              },
      });

  wgpuRenderPassEncoderEnd(resolve_pass);
}

void render_pass_draw_monosample(RenderPass pass_list[RENDER_PASS_COUNT],
                                 RenderPassLayout *pass_layout,
                                 WGPUTextureView *shared_color_view,
                                 WGPUTextureView *shared_depth_view,
                                 WGPUTextureView *swapchain_view,
                                 WGPUCommandEncoder *encoder) {

  // Travese passes (Scene >> Gizmo >> ...)
  for (size_t i = 0; i < pass_layout->length; i++) {

    const RenderPassType type = pass_layout->entries[i].pass;
    // look-up renderer pass from the list depending on the type
    RenderPass *pass = &pass_list[type];

    // replace color attachment resolve target by current swapchain view since
    // no msaa
    pass->color.attachment.resolveTarget = *swapchain_view;

    // begin render pass
    pass->encoder = wgpuCommandEncoderBeginRenderPass(
        *encoder, &(WGPURenderPassDescriptor){
                      .label = pass->label,
                      .colorAttachmentCount = 1,
                      .colorAttachments = &pass->color.attachment,
                      .depthStencilAttachment = &pass->depth.attachment,
                  });

    // draw layout (mesh > topo > shader)
    render_pass_draw_layout(pass_layout, &pass->encoder);

    // end render pass
    wgpuRenderPassEncoderEnd(pass->encoder);
  }
}
