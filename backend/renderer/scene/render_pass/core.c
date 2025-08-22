#include "core.h"
#include "draw.h"
#include "texture.h"

#include "webgpu/webgpu.h"
#include <string.h>

#include "../utils/system.h"

static inline void render_pass_draw_pass(RenderPass *, WGPUCommandEncoder);

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

  if (desc->color) {
    // assign color attributes
    render_pass->color.texture = desc->color->texture;

    WGPUTextureView color_view;
    if (desc->color->view == NULL &&
        desc->multisample > PipelineMultisampleCount_1x) {
      render_pass_create_multisampling_view(&render_pass->color.texture,
                                            &color_view, &texture_config);
    } else {
      color_view = desc->color->view;
    }

    render_pass->color.attachment = (WGPURenderPassColorAttachment){
        .view = color_view,
        .clearValue = desc->color->clear_value,
        .depthSlice = desc->color->depth_slice,
        .loadOp = desc->color->load_op,
        .storeOp = desc->color->store_op,
    };
  }

  if (desc->depth) {
    render_pass->depth.texture = desc->depth->texture;

    WGPUTextureView depth_view;
    if (desc->depth->view == NULL &&
        desc->multisample > PipelineMultisampleCount_1x) {
      render_pass_create_depth_view(&render_pass->depth.texture, &depth_view,
                                    &texture_config);
    } else {
      depth_view = desc->depth->view;
    }

    // assign depth
    render_pass->depth.attachment = (WGPURenderPassDepthStencilAttachment){
        .view = depth_view,
        .depthClearValue = desc->depth->clear_value,
        .depthReadOnly = desc->depth->read_only,
        .depthLoadOp = desc->depth->load_op,
        .depthStoreOp = desc->depth->store_op,
    };
  }
  
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
