#include "core.h"

#include <cglm/util.h>
#include <stdint.h>
#include <string.h>

#include "backend/logger.h"
#include "backend/postfx/core.h"
#include "draw.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/pipeline/render.h"
#include "texture.h"
#include "utils/dyli.h"
#include "utils/stli.h"
#include "webgpu/webgpu.h"

static inline void render_pass_draw_pass(RenderPass *, WGPUCommandEncoder);

void render_pass_create(RenderPass *render_pass,
                        const RenderPassCreateDescriptor *desc) {

  {
    // === assign core attributes ===

    render_pass->label = strdup(desc->label);
    render_pass->swapchain = desc->swapchain;
    render_pass->multisample = desc->multisample;
  }

  {
    // === assign draw callbacks ===

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
  }

  {
    // === create render textures ===
    if (desc->color) {

      RenderPassTextureDescriptor color_tex_config = {
          .height = desc->height,
          .width = desc->width,
          .multisample = render_pass->multisample,
          .format = desc->color->format,
      };

      // assign color attributes
      render_pass->color.texture = desc->color->texture;

      WGPUTextureView *main_color_view = &render_pass->color.views[0];

      if (desc->color->attachment.view == NULL) {

        render_pass_create_resolve_view(&render_pass->resolve_texture,
                                        &render_pass->resolve_view,
                                        &color_tex_config);

        if (desc->multisample > PipelineMultisampleCount_1x) {

          render_pass_create_multisampling_view(&render_pass->msaa_texture,
                                                &render_pass->msaa_view,
                                                &color_tex_config);

          render_pass->color.texture = render_pass->msaa_texture;
          *main_color_view = render_pass->msaa_view;
        }
      } else {
        *main_color_view = desc->color->attachment.view;
      }

      render_pass->color.attachment = desc->color->attachment;
      render_pass->color.attachment.view = *main_color_view;
      render_pass->color.views_length = 1;

      post_fx_init(&render_pass->post_fx, &(PostFxDescriptor){});

      if (render_pass->swapchain) { // onscreen blit effect
        post_fx_bind_texture_view(&render_pass->post_fx, PostFxType_Blit,
                                  render_pass->resolve_view);
        render_pass->color.attachment.resolveTarget = render_pass->resolve_view;
      }
    }

    if (desc->depth) {

      render_pass->depth.texture = desc->depth->texture;

      WGPUTextureView *main_depth_view = &render_pass->depth.views[0];
      if (desc->depth->attachment.view == NULL) {

        RenderPassTextureDescriptor depth_tex_config = {
            .height = desc->height,
            .width = desc->width,
            .multisample = render_pass->multisample,
            .format = desc->depth->format,
        };

        render_pass_create_depth_view(&render_pass->depth.texture,
                                      main_depth_view, &depth_tex_config);
      } else {
        *main_depth_view = desc->depth->attachment.view;
      }

      // assign depth
      render_pass->depth.attachment = desc->depth->attachment;
      render_pass->depth.attachment.view = *main_depth_view;
      render_pass->depth.views_length = 1;

    }
  }

  if (desc->draw_list)
    render_pass_draw_list_copy(desc->draw_list, &render_pass->draw_list);
}

void render_pass_list_create(RenderPassList *list,
                             const RenderPassListCreate *desc) {
  list->length = 0;
  list->swapchain = desc->swapchain;

  if (list->swapchain)
    list->draw_callback = render_pass_list_draw_onscreen_monosample;
  else
    list->draw_callback = render_pass_list_draw_offscreen;

  if (desc->multisample > PipelineMultisampleCount_1x && list->swapchain)
    list->draw_callback = render_pass_list_draw_onscreen_multisample;
}

void render_pass_list_insert_pass(RenderPassList *list,
                                  const RenderPassListInsert *desc) {

  if (list->length == RENDER_PASS_MAX_DRAW_LIST) {
    logger_add(LoggerFlag_Warning,
               "Render pass list reached maxed capacity (%d)",
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
                    L Stored Mesh List * (linked with scene)
                    L Drawn Mesh List
                    L Shader Callback
                    L Topo Callback

   1 - Solid config
   2 - Wireframe config
   3 - Boundbox config

   By following this order, we can simply map the right array entry depending on
   the scene render mode.
 */
void render_pass_draw_list_copy(const RenderPassDrawListDescriptor *src,
                                RenderPassDrawList *dest) {

  size_t length = glm_imin(src->length, RENDER_PASS_MAX_DRAW_LIST);
  dest->length = length;

  for (size_t i = 0; i < length; i++) {

    const RenderPassDrawLayoutDescriptor *s = &src->entries[i];
    RenderPassDrawLayout *d = &dest->entries[i];

    d->shader = s->shader;
    d->topology_callback = s->topology_callback;
    d->mesh_preprocessor_callback = s->mesh_preprocessor_callback;
    d->mesh_preprocessor_data = s->mesh_preprocessor_data;
    d->src_meshes = s->meshes;

    // initialize the drawn_meshes for each passes
    mesh_ref_list_create_and_copy(s->meshes, &d->drawn_meshes);
  }
}

RenderPassStatus render_pass_update_preprocessor_data(RenderPass *pass,
                                                      uint8_t index,
                                                      void *data) {
  if (index > pass->draw_list.length) {
    logger_add(LoggerFlag_Warning,
               "Trying to update an out of bound (%d) render pass "
               "preprocessor data. Target render pass has %lu draw lists.",
               index, pass->draw_list.length);
    return RenderPassStatus_OutOfBoundDrawIndex;
  }

  pass->draw_list.entries[index].mesh_preprocessor_data = data;

  return RenderPassStatus_Success;
}

RenderPassStatus render_pass_update_all_preprocessor_data(RenderPass *pass,
                                                          void *data) {

  for (size_t i = 0; i < pass->draw_list.length; i++)
    if (pass->draw_list.entries[i].mesh_preprocessor_callback)
      pass->draw_list.entries[i].mesh_preprocessor_data = data;

  return RenderPassStatus_Success;
}

StaticListStatus render_pass_view_color_insert(RenderPass *pass,
                                               WGPUTextureView view) {

  return stli_insert((void *)pass->color.views, RENDER_PASS_VIEW_CAPACITY,
                     &pass->color.views_length, sizeof(WGPUTextureView),
                     (void *)&view, "Render Pass Color View List");
}

StaticListStatus render_pass_view_depth_insert(RenderPass *pass,
                                               WGPUTextureView view) {
  return stli_insert((void *)pass->depth.views, RENDER_PASS_VIEW_CAPACITY,
                     &pass->depth.views_length, sizeof(WGPUTextureView),
                     (void *)&view, "Render Pass Depth View List");
}

StaticListStatus render_pass_view_color_remove(RenderPass *pass,
                                               WGPUTextureView view) {
  return stli_remove((void *)pass->color.views, &pass->color.views_length,
                     sizeof(WGPUTextureView), (void *)view,
                     "Render Pass Color View List");
}
StaticListStatus render_pass_view_depth_remove(RenderPass *pass,
                                               WGPUTextureView view) {
  return stli_remove((void *)pass->depth.views, &pass->depth.views_length,
                     sizeof(WGPUTextureView), (void *)view,
                     "Render Pass Color View List");
}

WGPUTextureView render_pass_view_color(RenderPass *pass, size_t index) {
  return pass->color.views[index];
}
WGPUTextureView render_pass_view_depth(RenderPass *pass, size_t index) {
  return pass->depth.views[index];
}
