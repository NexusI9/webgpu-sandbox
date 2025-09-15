#include "core.h"

#include <stdint.h>
#include <string.h>
#include <cglm/util.h>

#include "draw.h"
#include "texture.h"
#include "webgpu/webgpu.h"
#include "utils/system.h"
#include "runtime/mesh/ref_list.h"
#include "utils/dyli.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/core.h"
#include "utils/stli.h"

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

    WGPUTextureView *main_color_view = &render_pass->color.views[0];
    if (desc->color->view == NULL &&
        desc->multisample > PipelineMultisampleCount_1x) {
      render_pass_create_multisampling_view(&render_pass->color.texture,
                                            main_color_view, &texture_config);
    } else {
      *main_color_view = desc->color->view;
    }

    render_pass->color.attachment = (WGPURenderPassColorAttachment){
        .view = *main_color_view,
        .clearValue = desc->color->clear_value,
        .depthSlice = desc->color->depth_slice,
        .loadOp = desc->color->load_op,
        .storeOp = desc->color->store_op,
    };

    render_pass->color.views_length = 1;
  }

  if (desc->depth) {
    render_pass->depth.texture = desc->depth->texture;

    WGPUTextureView *main_depth_view = &render_pass->depth.views[0];
    if (desc->depth->view == NULL) {
      render_pass_create_depth_view(&render_pass->depth.texture,
                                    main_depth_view, &texture_config);
    } else {
      *main_depth_view = desc->depth->view;
    }

    // assign depth
    render_pass->depth.attachment = (WGPURenderPassDepthStencilAttachment){
        .view = *main_depth_view,
        .depthClearValue = desc->depth->clear_value,
        .depthReadOnly = desc->depth->read_only,
        .depthLoadOp = desc->depth->load_op,
        .depthStoreOp = desc->depth->store_op,
    };

    render_pass->depth.views_length = 1;
  }

  /*

    === copy draw list ===

   */
  if (desc->draw_list)
    render_pass_draw_list_copy(desc->draw_list, &render_pass->draw_list);
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
    VERBOSE_WARNING("Trying to update an out of bound (%d) render pass "
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

/*
  Target the right render pass draw layout based on the provided ref list
  pointer
 */
RenderPassDrawLayout *
render_pass_find_draw_layout_from_mesh(RenderPassDrawList *list,
                                       const Mesh *mesh) {

  for (uint16_t i = 0; i < list->length; i++) {
    const MeshRefList *ref_list = list->entries[i].src_meshes;
    if (mesh_ref_list_find(ref_list, mesh, NULL) != NULL)
      return &list->entries[i];
  }

  return NULL;
}

RenderPassDrawLayout *render_pass_find_draw_layout_from_mesh_ref_list(
    RenderPassDrawList *list, const MeshRefList *target_list) {

  for (uint16_t i = 0; i < list->length; i++)
    if (list->entries[i].src_meshes == target_list)
      return &list->entries[i];

  return NULL;
}

/**
   Sync the source mesh list with the actual draw list of the pass.
   We cannot draw directly the source mesh list (linked from the scene
   pipeline) because in some cases we need to hide of show some meshes in
   individual render pass.

   As instance, for probe reflection we may want to prevent self reflection
   and need to remove somes meshes from the pass. However if we remove those
   meshes from the source mesh list, then it means we also remove it from ALL
   the other passes that uses this same source list, which is not what we want
   (we still want them to be rendered on the main or shadow pass).

   Thus each draw list has two mesh list:
   1. the source mesh (const): which is the Source Of Truth, the actually list
   linked from thescene pipeline (Lit/ Unlit)
   2. the draw mesh: the more dynamic list from which we can enable or disable
   some meshes from the main list.

                      .----------.----------.----------.----------.----------.
   Scene Pipeline:    |  Mesh 1  |  Mesh 2  |  Mesh 3  |  Mesh 4  |  Mesh 5  |
                      '----------'----------'----------'----------'----------'
                                         └[ Linked ]┐
                      .----------.----------.----------.----------.----------.
   Source list:       |  Mesh 1  |  Mesh 2  |  Mesh 3  |  Mesh 4  |  Mesh 5  |
                      '----------'----------'----------'----------'----------'
                      .----------.----------.----------.
   Draw list:         |  Mesh 1  |  Mesh 3  |  Mesh 4  |
                      '----------'----------'----------'

   The caveats to this double layer list is that we need to make sure to sync
   the draw list when we add or remove mesh from the scene.
 */
RenderPassStatus render_pass_draw_list_enable_mesh(RenderPass *pass, Mesh *mesh,
                                                   const MeshRefList *reflist) {

  RenderPassDrawLayout *target_layout = NULL;
  {
    if (reflist != NULL)
      target_layout = render_pass_find_draw_layout_from_mesh_ref_list(
          &pass->draw_list, reflist);
    else
      target_layout =
          render_pass_find_draw_layout_from_mesh(&pass->draw_list, mesh);
  }

  {
    if (target_layout == NULL)
      return RenderPassStatus_LayoutUnfound;

    if (mesh_ref_list_insert(&target_layout->drawn_meshes, mesh) !=
        DynamicListStatus_Success)
      return RenderPassStatus_DrawListUpdateError;
  }

  return RenderPassStatus_Success;
}

RenderPassStatus
render_pass_draw_list_disable_mesh(RenderPass *pass, Mesh *mesh,
                                   const MeshRefList *reflist) {

  RenderPassDrawLayout *target_layout = NULL;
  {
    if (reflist != NULL)
      target_layout = render_pass_find_draw_layout_from_mesh_ref_list(
          &pass->draw_list, reflist);
    else
      target_layout =
          render_pass_find_draw_layout_from_mesh(&pass->draw_list, mesh);
  }

  {
    if (target_layout == NULL)
      return RenderPassStatus_LayoutUnfound;

    else if (mesh_ref_list_remove(&target_layout->drawn_meshes, mesh) !=
             DynamicListStatus_Success)
      return RenderPassStatus_DrawListUpdateError;
  }

  return RenderPassStatus_Success;
}

RenderPassStatus
render_pass_list_draw_list_enable_mesh(RenderPassList *list, Mesh *mesh,
                                       const MeshRefList *reflist) {

  for (uint8_t i = 0; i < list->length; i++)
    render_pass_draw_list_enable_mesh(&list->passes[i], mesh, reflist);

  return RenderPassStatus_Success;
}

RenderPassStatus
render_pass_list_draw_list_disable_mesh(RenderPassList *list, Mesh *mesh,
                                        const MeshRefList *reflist) {
  for (uint8_t i = 0; i < list->length; i++)
    render_pass_draw_list_disable_mesh(&list->passes[i], mesh, reflist);

  return RenderPassStatus_Success;
}

RenderPassStatus render_pass_draw_list_enable_mesh_ref_list(
    RenderPass *pass, MeshRefList *meshes, const MeshRefList *reflist) {

  for (size_t i = 0; i < meshes->length; i++)
    render_pass_draw_list_enable_mesh(pass, meshes->entries[i], reflist);

  return RenderPassStatus_Success;
}

RenderPassStatus render_pass_draw_list_disable_mesh_ref_list(
    RenderPass *pass, MeshRefList *meshes, const MeshRefList *reflist) {

  for (size_t i = 0; i < meshes->length; i++)
    render_pass_draw_list_disable_mesh(pass, meshes->entries[i], reflist);

  return RenderPassStatus_Success;
}

RenderPassStatus render_pass_list_draw_list_enable_mesh_ref_list(
    RenderPassList *list, MeshRefList *meshes, const MeshRefList *reflist) {

  for (uint8_t i = 0; i < list->length; i++)
    for (size_t j = 0; j < meshes->length; j++)
      render_pass_draw_list_enable_mesh(&list->passes[i], meshes->entries[j],
                                        reflist);

  return RenderPassStatus_Success;
}

RenderPassStatus render_pass_list_draw_list_disable_mesh_ref_list(
    RenderPassList *list, MeshRefList *meshes, const MeshRefList *reflist) {

  for (uint8_t i = 0; i < list->length; i++)
    for (size_t j = 0; j < meshes->length; j++)
      render_pass_draw_list_enable_mesh(&list->passes[i], meshes->entries[j],
                                        reflist);

  return RenderPassStatus_Success;
}

static int t = 0;
RenderPassStatus render_pass_draw_list_enable_all(RenderPass *pass) {

  for (uint16_t i = 0; i < pass->draw_list.length; i++) {
    RenderPassDrawLayout *layout = &pass->draw_list.entries[i];
    const MeshRefList *src = layout->src_meshes;
    MeshRefList *dest = &layout->drawn_meshes;

    dyli_replace((void *)src->entries, src->length, (void *)&dest->entries,
                 &dest->capacity, &dest->length, sizeof(Mesh *),
                 "Render pass draw layout");
  }

  return RenderPassStatus_Success;
}

RenderPassStatus render_pass_draw_list_disable_all(RenderPass *pass) {

  for (uint16_t i = 0; i < pass->draw_list.length; i++) {
    RenderPassDrawLayout *layout = &pass->draw_list.entries[i];
    MeshRefList *dest = &layout->drawn_meshes;

    dyli_empty((void *)dest->entries, &dest->length, sizeof(Mesh *));
  }

  return RenderPassStatus_Success;
}

RenderPassStatus render_pass_list_draw_list_disable_all(RenderPassList *list) {

  for (uint16_t i = 0; i < list->length; i++)
    render_pass_draw_list_disable_all(&list->passes[i]);

  return RenderPassStatus_Success;
}

RenderPassStatus render_pass_list_draw_list_enable_all(RenderPassList *list) {

  for (uint16_t i = 0; i < list->length; i++)
    render_pass_draw_list_enable_all(&list->passes[i]);

  return RenderPassStatus_Success;
}
