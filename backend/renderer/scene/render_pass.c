#include "render_pass.h"
#include "webgpu/webgpu.h"
#include <string.h>

#include "../utils/system.h"

static void render_pass_create_color_multisample(RenderPass *);
static void render_pass_create_color_monosample(RenderPass *);

static inline void render_pass_draw_pass(RenderPass *, WGPUCommandEncoder);

static inline void
render_pass_create_multisampling_view(WGPUTexture *, WGPUTextureView *,
                                      const RenderPassTextureDescriptor *);

static inline void
render_pass_create_depth_view(WGPUTexture *, WGPUTextureView *,
                              const RenderPassTextureDescriptor *);

void render_pass_create(RenderPass *render_pass,
                        const RenderPassCreateDescriptor *desc) {

  /*

    === assign core attributes ===

    */
  render_pass->label = strdup(desc->label);
  render_pass->device = desc->device;
  render_pass->queue = desc->queue;
  render_pass->swapchain = desc->swapchain;
  render_pass->multisample = desc->multisample;

  /*

    === assign draw callbacks ===

   */

  // on screen drawing
  if (render_pass->swapchain) {
    // define callback based on multisample
    switch (desc->multisample) {

    case PipelineMultisampleCount_4x:
      render_pass->draw_callback = render_pass_draw_onscreen_multisample;
      break;

    case PipelineMultisampleCount_1x:
    default:
      render_pass->draw_callback = render_pass_draw_onscreen_monosample;
      break;
    }

  } else {
    // off screen rendering (common drawing method no matter the msaa)
    render_pass->draw_callback = render_pass_draw_offscreen;
  }

  /*

     === create render textures ===

   */
  RenderPassTextureDescriptor texture_config = {
      .device = render_pass->device,
      .height = desc->height,
      .width = desc->width,
      .multisample = render_pass->multisample,
  };

  // assign color attributes
  WGPUTextureView color_view;
  if (desc->color.view)
    color_view = desc->color.view;
  else if (desc->multisample > PipelineMultisampleCount_1x)
    render_pass_create_multisampling_view(&render_pass->color.texture,
                                          &color_view, &texture_config);

  render_pass->color.attachment = (WGPURenderPassColorAttachment){
      .view = color_view,
      .clearValue = desc->color.clear_value,
      .depthSlice = desc->color.depth_slice,
      .loadOp = desc->color.load_op,
      .storeOp = desc->color.store_op,
  };

  WGPUTextureView depth_view;
  if (desc->depth.view == NULL)
    render_pass_create_depth_view(&render_pass->depth.texture, &depth_view,
                                  &texture_config);
  else
    depth_view = desc->depth.view;

  // assign depth
  render_pass->depth.attachment = (WGPURenderPassDepthStencilAttachment){
      .view = depth_view,
      .depthClearValue = desc->depth.clear_value,
      .depthReadOnly = desc->depth.read_only,
      .depthLoadOp = desc->depth.load_op,
      .depthStoreOp = desc->depth.store_op,
  };

  /*

    === copy draw list ===

   */
  if (desc->draw_list)
    render_pass_set_draw_list(render_pass, desc->draw_list);
}

void render_pass_list_create(RenderPassList *list,
                             const RenderPassListCreate *desc) {
  list->length = 0;
  list->device = desc->device;
  list->queue = desc->queue;
  list->swapchain = desc->swapchain;

  if (list->swapchain)
    list->draw_callback = render_pass_list_draw_onscreen_monosample;
  else
    list->draw_callback = render_pass_list_draw_offscreen;

  if (desc->multisample > PipelineMultisampleCount_1x) {
    render_pass_create_multisampling_view(&list->resolve_texture,
                                          &list->resolve_view,
                                          &(RenderPassTextureDescriptor){
                                              .device = list->device,
                                              .height = desc->height,
                                              .width = desc->width,
                                              .multisample = desc->multisample,
                                          });
    if (list->swapchain)
      list->draw_callback = render_pass_list_draw_onscreen_multisample;
  }
}

void render_pass_list_insert_pass(RenderPassList *list,
                                  const RenderPassListInsert *desc) {

  if (list->length == RENDER_PASS_MAX_DRAW_LIST) {
    VERBOSE_WARNING("Render pass list reached maxed capacity (%d)",
                    RENDER_PASS_MAX_DRAW_LIST);
    return;
  }

  render_pass_create(&list->passes[list->length++],
                     &(RenderPassCreateDescriptor){

                         .label = desc->label,
                         .draw_list = desc->draw_list,
                         .color = desc->color,
                         .depth = desc->depth,
                         .width = desc->width,
                         .height = desc->height,
                         .multisample = desc->multisample,

                         // list inherited properties
                         .device = list->device,
                         .queue = list->queue,
                         .swapchain = list->swapchain,
                     });
}

/**
   Create scene renderer draw config, which basically is an array of callback
   functions and mesh referecences list lists that will be picked during the
   draw loop.

   Basically for each draw call we require a "topology callback" and a
   "shader callback" to define which topology and shader we want to draw for
   each mesh.

   Note that the order of the array is relative to the SceneRendererMode:

   0 - Texture config
          L Render Pass 1
          L Render Pass 2
               L Length
               L Draw Layouts[]
                    L Mesh List
                    L Shader Callback
                    L Topo Callback

   1 - Solid config
   2 - Wireframe config
   3 - Boundbox config

   By following this order, we can simply map the right array entry depending on
   the scene render mode.
 */
void render_pass_set_draw_list(RenderPass *pass,
                               const RenderPassDrawList *draw_list) {

  size_t length = glm_imin(draw_list->length, RENDER_PASS_MAX_DRAW_LIST);
  pass->draw_list.length = length;
  memcpy(pass->draw_list.entries, draw_list->entries,
         sizeof(RenderPassDrawLayout) * length);
}

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
        list->mesh_preprocessor;
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
  WGPUTextureView src_depth_view = pass->color.attachment.view;

  if (overrides->color)
    pass->color.attachment.view = overrides->color;

  if (overrides->depth)
    pass->depth.attachment.view = overrides->depth;

  pass->draw_callback(pass);

  wgpuTextureViewRelease(pass->color.attachment.view);
  wgpuTextureViewRelease(pass->depth.attachment.view);

  pass->color.attachment.view = src_color_view;
  pass->depth.attachment.view = src_depth_view;
}

/**




   ▗▄▄▄▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▖ ▗▖▗▄▄▖ ▗▄▄▄▖ ▗▄▄▖
     █  ▐▌    ▝▚▞▘   █  ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌
     █  ▐▛▀▀▘  ▐▌    █  ▐▌ ▐▌▐▛▀▚▖▐▛▀▀▘ ▝▀▚▖
     █  ▐▙▄▄▖▗▞▘▝▚▖  █  ▝▚▄▞▘▐▌ ▐▌▐▙▄▄▖▗▄▄▞▘

   Create the texture and texture view for the multisampling rendering.




 */
void render_pass_create_multisampling_view(
    WGPUTexture *texture, WGPUTextureView *view,
    const RenderPassTextureDescriptor *desc) {

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

void render_pass_create_depth_view(WGPUTexture *texture, WGPUTextureView *view,
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
