#ifndef _RENDER_PASS_VISBILITY_H_
#define _RENDER_PASS_VISBILITY_H_

#include "core.h"

/* ===  Visbility manager ===

  The below functions try to provide an interface to help hidding or showing
  meshes from the render passes.

  Render Pass are composed of  "Layouts" lists.
  Layouts are basically meshes coupled with topology (wireframe, solid,
  boundbox) and shader (texture, solid, wireframe) callbacks.
  This trio is combined and used to be drawn together.

  Think of each layout as:

        "Draw this [mesh] with this [topology] and thi [shader]"

  Each layout is composed of two mesh lists to be drawn:

  - The Source: a constant list that is a shared reference amongst many layout.
    The source list usually comes from the scene pipelines (Lit meshes,
    unlit, shadowed) and can be seen as a fixed stagging list.

  - The Drawn: a more dynamic list in which we actually decide which meshes from
    the source list we want to draw from.

   This double layering ensure consistency accross multiple layouts by always
   referencing a same common source list while having the flexbility
   "per-layout" to render or not some meshes independtently.


   For information, priorly the engine used a "one layer" layout system, meaning
   each layout was directly linked and would show the source list meshes.
   However we quickly noticed that sometime we may want to hide the mesh from a
   layout but not from the whole shared source list.

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
                      '-----.----'----------'----.----'-----.----'----------'
                            |          .---------'.---------'
                      .-----'----.-----'----.----'-----.
   Draw list:         |  Mesh 1  |  Mesh 3  |  Mesh 4  |
                      '----------'----------'----------'

   The caveats to this double layer list is that we need to make sure to sync
   the draw list when we add or remove mesh from the scene.

   ==========================================================================
   ==========================================================================

   Notice some of the interface function may be slower that others du to their
  query scope.

   Render pass list  ------------------- (level 1)
        L Render Pass ------------------ (level 2)
             L Layout ------------------ (level 3)
                 L Mesh list
                      L Mesh



   .--- Render pass list ----------------------------------------------------.
   |                                                                         |
   |  .--- Render pass 1 ----------------.--- Render pass 2 --------------.  |
   |  |                                  |                                |  |
   |  | .--------.--------.--------.-----| .--------.--------.--------.---|  |
   |  | | Layout | Layout | Layout | Lay | | Layout | Layout | Layout | L |  |
   |  | '--------'--------'--------'-----| '--------'--------'--------'---|  |
   |  |                                  |                                |  |
   |  '----------------------------------'--------------------------------'  |
   |                                                                         |
   '-------------------------------------------------------------------------'

   As we can see, by targetting the layout directly we drastically reduce the
   query scope.

 */

/*
  Target the right render pass draw layout based on the provided ref list
  pointer
 */
static inline RenderPassDrawLayout *
render_pass_find_layout_from_mesh(RenderPass *pass, const Mesh *mesh) {

  for (uint16_t i = 0; i < pass->draw_list.length; i++) {
    const MeshRefList *ref_list = pass->draw_list.entries[i].src_meshes;
    if (mesh_ref_list_find(ref_list, mesh, NULL) != NULL)
      return &pass->draw_list.entries[i];
  }

  return NULL;
}

static inline RenderPassDrawLayout *
render_pass_find_layout_from_source_list(RenderPass *pass,
                                         const MeshRefList *source_list) {

  for (uint16_t i = 0; i < pass->draw_list.length; i++)
    if (pass->draw_list.entries[i].src_meshes == source_list)
      return &pass->draw_list.entries[i];

  return NULL;
}

/**
             .============ LEVEL 3 ============.

   ▗▄▄▖ ▗▄▄▄▖▗▖  ▗▖▗▄▄▄ ▗▄▄▄▖▗▄▄▖     ▗▖    ▗▄▖▗▖  ▗▖▗▄▖ ▗▖ ▗▖▗▄▄▄▖
   ▐▌ ▐▌▐▌   ▐▛▚▖▐▌▐▌  █▐▌   ▐▌ ▐▌    ▐▌   ▐▌ ▐▌▝▚▞▘▐▌ ▐▌▐▌ ▐▌  █
   ▐▛▀▚▖▐▛▀▀▘▐▌ ▝▜▌▐▌  █▐▛▀▀▘▐▛▀▚▖    ▐▌   ▐▛▀▜▌ ▐▌ ▐▌ ▐▌▐▌ ▐▌  █
   ▐▌ ▐▌▐▙▄▄▖▐▌  ▐▌▐▙▄▄▀▐▙▄▄▖▐▌ ▐▌    ▐▙▄▄▖▐▌ ▐▌ ▐▌ ▝▚▄▞▘▝▚▄▞▘  █


 */

static inline RenderPassStatus
render_pass_layout_enable_mesh(RenderPassDrawLayout *layout, Mesh *mesh) {

  if (mesh_ref_list_insert(&layout->drawn_meshes, mesh) != NULL)
    return RenderPassStatus_DrawListUpdateError;

  return RenderPassStatus_Success;
}

static inline RenderPassStatus
render_pass_layout_disable_mesh(RenderPassDrawLayout *layout, Mesh *mesh) {

  if (mesh_ref_list_remove(&layout->drawn_meshes, mesh) !=
      DynamicListStatus_Success)
    return RenderPassStatus_DrawListUpdateError;

  return RenderPassStatus_Success;
}

static inline RenderPassStatus
render_pass_layout_enable_mesh_ref_list(RenderPassDrawLayout *layout,
                                        MeshRefList *list) {

  for (size_t i = 0; i < list->length; i++)
    render_pass_layout_enable_mesh(layout, list->entries[i]);

  return RenderPassStatus_Success;
}

static inline RenderPassStatus
render_pass_layout_disable_mesh_ref_list(RenderPassDrawLayout *layout,
                                         MeshRefList *list) {

  for (size_t i = 0; i < list->length; i++)
    render_pass_layout_enable_mesh(layout, list->entries[i]);

  return RenderPassStatus_Success;
}

static inline RenderPassStatus
render_pass_layout_enable_all_mesh(RenderPassDrawLayout *layout) {

  const MeshRefList *src = layout->src_meshes;
  MeshRefList *dest = &layout->drawn_meshes;

  dyli_replace((void *)src->entries, src->length, (void **)&dest->entries,
               &dest->capacity, &dest->length, sizeof(Mesh *),
               "Render pass draw layout");

  return RenderPassStatus_Success;
}

static inline RenderPassStatus
render_pass_layout_disable_all_mesh(RenderPassDrawLayout *layout) {
  MeshRefList *dest = &layout->drawn_meshes;
  dyli_empty((void *)dest->entries, &dest->length, sizeof(Mesh *));
  return RenderPassStatus_Success;
}


// TODO: find a way to make the ownership sharing safer.
static inline RenderPassStatus
render_pass_layout_swap_draw_list(RenderPassDrawLayout *layout,
                                  MeshRefList *list) {

  layout->drawn_meshes.entries = list->entries;
  layout->drawn_meshes.length = list->length;
  layout->drawn_meshes.capacity = list->capacity;

  return RenderPassStatus_Success;
}

/**
            .============ LEVEL 2 ============.

   ▗▄▄▖ ▗▄▄▄▖▗▖  ▗▖▗▄▄▄ ▗▄▄▄▖▗▄▄▖     ▗▄▄▖  ▗▄▖  ▗▄▄▖ ▗▄▄▖
   ▐▌ ▐▌▐▌   ▐▛▚▖▐▌▐▌  █▐▌   ▐▌ ▐▌    ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌
   ▐▛▀▚▖▐▛▀▀▘▐▌ ▝▜▌▐▌  █▐▛▀▀▘▐▛▀▚▖    ▐▛▀▘ ▐▛▀▜▌ ▝▀▚▖ ▝▀▚▖
   ▐▌ ▐▌▐▙▄▄▖▐▌  ▐▌▐▙▄▄▀▐▙▄▄▖▐▌ ▐▌    ▐▌   ▐▌ ▐▌▗▄▄▞▘▗▄▄▞▘


 */

static inline RenderPassStatus render_pass_enable_mesh(RenderPass *pass,
                                                       Mesh *mesh) {

  RenderPassDrawLayout *layout = render_pass_find_layout_from_mesh(pass, mesh);

  {
    if (layout == NULL)
      return RenderPassStatus_LayoutUnfound;

    return render_pass_layout_enable_mesh(layout, mesh);
  }

  return RenderPassStatus_Success;
}

static inline RenderPassStatus render_pass_disable_mesh(RenderPass *pass,
                                                        Mesh *mesh) {

  RenderPassDrawLayout *layout = render_pass_find_layout_from_mesh(pass, mesh);

  {
    if (layout == NULL)
      return RenderPassStatus_LayoutUnfound;

    return render_pass_layout_disable_mesh(layout, mesh);
  }

  return RenderPassStatus_Success;
}

static inline RenderPassStatus
render_pass_enable_mesh_ref_list(RenderPass *pass, MeshRefList *meshes) {

  for (size_t i = 0; i < meshes->length; i++)
    render_pass_enable_mesh(pass, meshes->entries[i]);

  return RenderPassStatus_Success;
}

static inline RenderPassStatus
render_pass_disable_mesh_ref_list(RenderPass *pass, MeshRefList *meshes) {

  for (size_t i = 0; i < meshes->length; i++)
    render_pass_disable_mesh(pass, meshes->entries[i]);

  return RenderPassStatus_Success;
}

static inline RenderPassStatus render_pass_enable_all_mesh(RenderPass *pass) {

  for (uint16_t i = 0; i < pass->draw_list.length; i++) {
    RenderPassDrawLayout *layout = &pass->draw_list.entries[i];
    render_pass_layout_enable_all_mesh(layout);
  }

  return RenderPassStatus_Success;
}

static inline RenderPassStatus render_pass_disable_all_mesh(RenderPass *pass) {

  for (uint16_t i = 0; i < pass->draw_list.length; i++) {
    RenderPassDrawLayout *layout = &pass->draw_list.entries[i];
    render_pass_layout_disable_all_mesh(layout);
  }

  return RenderPassStatus_Success;
}

/**
           .============ LEVEL 1 ============.

   ▗▄▄▖ ▗▄▄▄▖▗▖  ▗▖▗▄▄▄ ▗▄▄▄▖▗▄▄▖     ▗▖   ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖
   ▐▌ ▐▌▐▌   ▐▛▚▖▐▌▐▌  █▐▌   ▐▌ ▐▌    ▐▌     █  ▐▌     █
   ▐▛▀▚▖▐▛▀▀▘▐▌ ▝▜▌▐▌  █▐▛▀▀▘▐▛▀▚▖    ▐▌     █   ▝▀▚▖  █
   ▐▌ ▐▌▐▙▄▄▖▐▌  ▐▌▐▙▄▄▀▐▙▄▄▖▐▌ ▐▌    ▐▙▄▄▖▗▄█▄▖▗▄▄▞▘  █

 */

static inline RenderPassStatus
render_pass_list_enable_mesh(RenderPassList *list, Mesh *mesh) {

  for (uint8_t i = 0; i < list->length; i++)
    render_pass_enable_mesh(&list->passes[i], mesh);

  return RenderPassStatus_Success;
}

static inline RenderPassStatus
render_pass_list_disable_mesh(RenderPassList *list, Mesh *mesh) {
  for (uint8_t i = 0; i < list->length; i++)
    render_pass_disable_mesh(&list->passes[i], mesh);

  return RenderPassStatus_Success;
}

static inline RenderPassStatus
render_pass_list_enable_mesh_ref_list(RenderPassList *list,
                                      MeshRefList *meshes) {

  for (uint8_t i = 0; i < list->length; i++)
    for (size_t j = 0; j < meshes->length; j++)
      render_pass_enable_mesh(&list->passes[i], meshes->entries[j]);

  return RenderPassStatus_Success;
}

static inline RenderPassStatus
render_pass_list_disable_mesh_ref_list(RenderPassList *list,
                                       MeshRefList *meshes) {

  for (uint8_t i = 0; i < list->length; i++)
    for (size_t j = 0; j < meshes->length; j++)
      render_pass_enable_mesh(&list->passes[i], meshes->entries[j]);

  return RenderPassStatus_Success;
}

static inline RenderPassStatus
render_pass_list_disable_all_mesh(RenderPassList *list) {

  for (uint16_t i = 0; i < list->length; i++)
    render_pass_disable_all_mesh(&list->passes[i]);

  return RenderPassStatus_Success;
}

static inline RenderPassStatus
render_pass_list_enable_all_mesh(RenderPassList *list) {

  for (uint16_t i = 0; i < list->length; i++)
    render_pass_enable_all_mesh(&list->passes[i]);

  return RenderPassStatus_Success;
}

#endif
