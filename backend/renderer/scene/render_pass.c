#include "render_pass.h"
#include "webgpu/webgpu.h"
#include <string.h>

static void render_pass_create_color_multisample(RenderPass *);
static void render_pass_create_color_monosample(RenderPass *);

static void render_pass_draw_layout(RenderPassDrawList *,
                                    WGPURenderPassEncoder *);

static void render_pass_draw_monosample(RenderPass *, RenderPassLayout *,
                                        WGPUTextureView, WGPUCommandEncoder);

static void render_pass_draw_multisample(RenderPass *, RenderPassLayout *,
                                         WGPUTextureView, WGPUCommandEncoder);

static inline void
render_pass_create_multisampling_view(WGPUTextureView *,
                                      const RenderPassTextureDescriptor *);

static inline void
render_pass_create_depth_view(WGPUTextureView *,
                              const RenderPassTextureDescriptor *);

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
  render_pass->device = desc->device;
  render_pass->queue = desc->queue;
  render_pass->height = desc->height;
  render_pass->width = desc->width;
  render_pass->swapchain = desc->swapchain;
  render_pass->multisample = desc->multisample;

  RenderPassTextureDescriptor texture_config = {
      .device = render_pass->device,
      .height = render_pass->height,
      .width = render_pass->width,
      .multisample = render_pass->multisample,
  };

  // assign color attributes
  WGPUTextureView color_view;
  if (desc->color.view)
    color_view = *desc->color.view;
  else if (desc->multisample != PipelineMultisampleCount_1x)
    render_pass_create_multisampling_view(&color_view, &texture_config);


  render_pass->color.attachment = (WGPURenderPassColorAttachment){
      .view = color_view,
      .clearValue = desc->color.clear_value,
      .depthSlice = desc->color.depth_slice,
      .loadOp = desc->color.load_op,
      .storeOp = desc->color.store_op,
  };

  WGPUTextureView depth_view;
  if (desc->depth.view == NULL)
    render_pass_create_depth_view(&depth_view, &texture_config);
  else
    depth_view = *desc->depth.view;

  // assign depth
  render_pass->depth.attachment = (WGPURenderPassDepthStencilAttachment){
      .view = depth_view,
      .depthClearValue = desc->depth.clear_value,
      .depthReadOnly = desc->depth.read_only,
      .depthLoadOp = desc->depth.load_op,
      .depthStoreOp = desc->depth.store_op,
  };
}

void render_pass_draw(RenderPass *pass, RenderPassDrawDescriptor *desc) {

  /*
  Create 1 command encoder for all the render passes.
  Encoder records GPU operations such as:
  - Texture upload
  - Buffer upload
  - Render passes
  - Compute passes
*/
  WGPUCommandEncoder render_encoder =
      wgpuDeviceCreateCommandEncoder(desc->device, NULL);

  // get swapchain view to be resolved
  WGPUTextureView swapchain_view =
      wgpuSwapChainGetCurrentTextureView(*pass->swapchain);

  draw_callback[pass->multisample](pass, desc->pass_layout, swapchain_view,
                                   render_encoder);

  // create command buffer
  WGPUCommandBuffer render_buffer =
      wgpuCommandEncoderFinish(render_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(desc->queue, 1, &render_buffer);

  // release all passes encoders
  for (size_t i = 0; i < desc->pass_layout->length; i++) {
    const RenderPassType type = desc->pass_layout->entries[i].pass;
    const RenderPass *p = &pass[type];
    wgpuRenderPassEncoderRelease(p->encoder);
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
void render_pass_draw_layout(RenderPassDrawList *draw_list,
                             WGPURenderPassEncoder *encoder) {

  // Draw meshes
  // loop through mesh lists and draw meshes
  for (size_t j = 0; j < draw_list->length; j++) {

    // retrieve layout
    RenderPassDrawLayout *layout = &draw_list->entries[j];
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
void render_pass_draw_multisample(RenderPass *pass_list,
                                  RenderPassLayout *pass_layout,
                                  WGPUTextureView swapchain_view,
                                  WGPUCommandEncoder encoder) {

  // Travese passes (Scene >> Gizmo >> ...)
  for (size_t i = 0; i < pass_layout->length; i++) {

    const RenderPassType type = pass_layout->entries[i].pass;
    // look-up renderer pass from the list depending on the type
    RenderPass *pass = &pass_list[type];

    // begin render pass
    pass->encoder = wgpuCommandEncoderBeginRenderPass(
        encoder, &(WGPURenderPassDescriptor){
                     .label = pass->label,
                     .colorAttachmentCount = 1,
                     .colorAttachments = &pass->color.attachment,
                     .depthStencilAttachment = &pass->depth.attachment,
                 });

    // draw layout list (mesh > topo > shader)
    RenderPassDrawList *draw_list = &pass_layout->entries[i];
    render_pass_draw_layout(draw_list, &pass->encoder);

    // end render pass
    wgpuRenderPassEncoderEnd(pass->encoder);
  }

  // resolve pass (MSAA only)
  WGPURenderPassEncoder resolve_pass = wgpuCommandEncoderBeginRenderPass(
      encoder,
      &(WGPURenderPassDescriptor){
          .label = "MSAA Resolve Pass",
          .colorAttachmentCount = 1,
          .colorAttachments =
              &(WGPURenderPassColorAttachment){
                  .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
                  // pass last pass 4x sample as view
                  .view =
                      pass_list[pass_layout->length - 1].color.attachment.view,
                  .resolveTarget = swapchain_view, // 1x sampled
                  .loadOp = WGPULoadOp_Load,
                  .storeOp = WGPUStoreOp_Store,
              },
      });

  wgpuRenderPassEncoderEnd(resolve_pass);
}

void render_pass_draw_monosample(RenderPass *pass_list,
                                 RenderPassLayout *pass_layout,
                                 WGPUTextureView swapchain_view,
                                 WGPUCommandEncoder encoder) {

  // Travese passes (Scene >> Gizmo >> ...)
  for (size_t i = 0; i < pass_layout->length; i++) {

    const RenderPassType type = pass_layout->entries[i].pass;
    // look-up renderer pass from the list depending on the type
    RenderPass *pass = &pass_list[type];

    // begin render pass
    pass->encoder = wgpuCommandEncoderBeginRenderPass(
        encoder, &(WGPURenderPassDescriptor){
                     .label = pass->label,
                     .colorAttachmentCount = 1,
                     .colorAttachments =
                         &(WGPURenderPassColorAttachment){
                             .view = swapchain_view, // replace with swapchain
                             .clearValue = pass->color.attachment.clearValue,
                             .depthSlice = pass->color.attachment.depthSlice,
                             .loadOp = pass->color.attachment.loadOp,
                             .storeOp = pass->color.attachment.storeOp,
                         },
                     .depthStencilAttachment = &pass->depth.attachment,
                 });

    // draw layout (mesh > topo > shader)
    RenderPassDrawList *draw_list = &pass_layout->entries[i];
    render_pass_draw_layout(draw_list, &pass->encoder);

    // end render pass
    wgpuRenderPassEncoderEnd(pass->encoder);
  }
}

/**
   Create the texture and texture view for the multisampling rendering.
 */
void render_pass_create_multisampling_view(
    WGPUTextureView *view, const RenderPassTextureDescriptor *desc) {

  if (desc->multisample == 0) {
    VERBOSE_WARNING("Multisample provided is not valid (%d), make sure the "
                    "render pass is correctly initialised.",
                    desc->multisample);
    return;
  }

  WGPUTexture msaa_texture = wgpuDeviceCreateTexture(
      desc->device,
      &(WGPUTextureDescriptor){
          .label = "MSAA Texture",
          .usage = WGPUTextureUsage_RenderAttachment,
          .size =
              (WGPUExtent3D){
                  .width = desc->width,
                  .height = desc->height,
                  .depthOrArrayLayers = 1,
              },
          .format = WGPUTextureFormat_BGRA8Unorm, // swapchain format
          .sampleCount = desc->multisample,
          .mipLevelCount = 1,
      });

  *view = wgpuTextureCreateView(msaa_texture, NULL);
}

void render_pass_create_depth_view(WGPUTextureView *view,
                                   const RenderPassTextureDescriptor *desc) {

  // Need to create a texture view for Z buffer stencil
  // by default set depth based on draw call order (first ones in
  // backgrounds...)
  // => Need to create a depth texture: a hidden buffer storing depth values for
  // each pixel
  WGPUTexture depthTexture = wgpuDeviceCreateTexture(
      desc->device,
      &(WGPUTextureDescriptor){
          .usage = WGPUTextureUsage_RenderAttachment, // used in rendering pass
          .size =
              (WGPUExtent3D){
                  .width = desc->width,
                  .height = desc->height,
                  .depthOrArrayLayers = 1,
              },
          .format =
              WGPUTextureFormat_Depth24Plus, // texture with 24bit-depth format
          .mipLevelCount = 1,
          .sampleCount = desc->multisample,
          .dimension = WGPUTextureDimension_2D,
      });

  *view = wgpuTextureCreateView(
      depthTexture,
      &(WGPUTextureViewDescriptor){
          .format = WGPUTextureFormat_Depth24Plus,
          .dimension = WGPUTextureViewDimension_2D,
          .baseMipLevel = 0,
          .mipLevelCount = 1, // match above texture
          .baseArrayLayer = 0,
          .arrayLayerCount = 1, // not using array texture (only 1)
          .aspect = WGPUTextureAspect_DepthOnly,
      });
}
