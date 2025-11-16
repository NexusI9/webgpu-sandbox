#include "visibility.h"
#include "backend/renderer/core.h"
#include "backend/renderer/render_pass/core.h"

/**

   .---------------------------------------------------------------------.
   |                ADD               |              REMOVE              |
   |---------------------------------------------------------------------|
   |   Add and Remove functions basically mounts, unmounts the mesh      |
   |   from the scene. Meaning they build the mesh shader internally     |
   |   and show it visually by adding it to the pipeline list.           |
   |   Those 2 functions should only be used for first and last instan-  |
   |   tiation of the mesh.                                              |
   |                                                                     |
   |   .------------ ⚙ ------------.    .------------ ◉ -------------.  |
   |   |    BUILD    |   UNBUILD    |    |     SHOW    |     HIDE     |  |
   |   |----------------------------|    |----------------------------|  |
   |   | Build and Unbuild function | => | Show and Hide functions    |  |
   |   | only handle the mesh       | => | operate at a visual level  |  |
   |   | internal binding. It does  | => | only. They only pop or push|  |
   |   | not visually add the mesh  | => | the mesh from the pipeline |  |
   |   | to the scene pipeline.     | => | array. However it's        |  |
   |   | Building only "prepares"   | => | important to make sure the |  |
   |   | the mesh for the drawcall. |    | mesh is Built priorly.     |  |
   |   '----------------------------'    '----------------------------'  |
   '---------------------------------------------------------------------'

 */

/**
   Show the mesh by pushing it to the pipeline ref list
 */
RendererStatus renderer_show_mesh(Renderer *rd,
                                  const RendererDrawMode draw_mode,
                                  const RendererLayer layers, Mesh *mesh) {

  for (uint8_t i = 0; i < RENDERER_DRAW_MODE_COUNT; i++)
    if (draw_mode & (1 << i)) {
      RenderPassList *plist =
          renderer_mesh_pass_list(rd, (RendererDrawMode)(1 << i));

      for (size_t j = 0; j < plist->length; j++) {
        if (layers & (1 << j)) {
          RenderPass *pass = &plist->passes[j];
          render_pass_enable_mesh(pass, mesh);
        }
      }
    }

  mesh_ref_list_remove(renderer_mesh_state(rd, RendererMeshStates_Hidden),
                       mesh);

  return RendererStatus_Success;
}

/**
   Hide the mesh by removing it from the pipelines ref list.
 */
RendererStatus renderer_hide_mesh(Renderer *rd,
                                  const RendererDrawMode draw_mode,
                                  const RendererLayer layers, Mesh *mesh) {

  for (uint8_t i = 0; i < RENDERER_DRAW_MODE_COUNT; i++)
    if (draw_mode & (1 << i)) {
      RenderPassList *plist =
          renderer_mesh_pass_list(rd, (RendererDrawMode)(1 << i));

      for (size_t j = 0; j < plist->length; j++) {
        if (layers & (1 << j)) {
          RenderPass *pass = &plist->passes[j];
          render_pass_disable_mesh(pass, mesh);
        }
      }
    }

  mesh_ref_list_insert(renderer_mesh_state(rd, RendererMeshStates_Hidden),
                       mesh);

  return RendererStatus_Success;
}

RendererStatus renderer_show_mesh_ref_list(Renderer *rd,
                                           const RendererDrawMode draw_mode,
                                           const RendererLayer layers,
                                           MeshRefList *list) {

  for (size_t i = 0; i < list->length; i++)
    renderer_show_mesh(rd, draw_mode, layers, list->entries[i]);

  return RendererStatus_Success;
}

RendererStatus renderer_hide_mesh_ref_list(Renderer *rd,
                                           const RendererDrawMode draw_mode,
                                           const RendererLayer layers,
                                           MeshRefList *list) {

  for (size_t i = 0; i < list->length; i++)
    renderer_hide_mesh(rd, draw_mode, layers, list->entries[i]);

  return RendererStatus_Success;
}

RendererStatus renderer_visibility_toggle_mesh(Renderer *rd,
                                               const RendererDrawMode draw_mode,
                                               const RendererLayer layers,
                                               Mesh *mesh) {

  if (mesh_ref_list_find(renderer_mesh_state(rd, RendererMeshStates_Hidden),
                         mesh, NULL)) {
    renderer_show_mesh(rd, draw_mode, layers, mesh);
    return RendererStatus_MeshVisible;
  } else {
    renderer_hide_mesh(rd, draw_mode, layers, mesh);
    return RendererStatus_MeshHidden;
  }
}

RendererStatus renderer_show_mesh_in_pipeline(Renderer *rd,
                                              const RenderPipelineType pipeline,
                                              Mesh *mesh) {

  RendererBatchMeshLists source_lists;
  renderer_batch_get_mesh_list_from_pipeline(&rd->batches, pipeline,
                                             &source_lists);

  // enable in all batch except selection related ones
  for (RendererDrawMode i = 0; i < RENDERER_DRAW_MODE_COUNT; i++) {

    RenderPassList *pass_list =
        renderer_mesh_pass_list(rd, (RendererDrawMode)(1 << i));

    for (size_t j = 0; j < pass_list->length; j++) {
      RenderPass *pass = &pass_list->passes[j];

      for (size_t k = 0; k < source_lists.length; k++) {
        RenderPassLayout *layout = render_pass_find_layout_from_source_list(
            pass, source_lists.entries[k]);

        if (layout)
          render_pass_layout_enable_mesh(layout, mesh);
      }

      render_pass_sync_drawn_layouts(pass);
    }
  }


  return RendererStatus_Success;
}

RendererStatus renderer_hide_mesh_in_pipeline(Renderer *rd,
                                              const RenderPipelineType pipeline,
                                              Mesh *mesh) {

  RendererBatchMeshLists source_lists;
  renderer_batch_get_mesh_list_from_pipeline(&rd->batches, pipeline,
                                             &source_lists);

  // enable in all batch except selection related ones
  for (RendererDrawMode i = 0; i < RENDERER_DRAW_MODE_COUNT; i++) {

    RenderPassList *pass_list =
        renderer_mesh_pass_list(rd, (RendererDrawMode)(1 << i));

    for (size_t j = 0; j < pass_list->length; j++) {
      RenderPass *pass = &pass_list->passes[j];

      for (size_t k = 0; k < source_lists.length; k++) {
        RenderPassLayout *layout = render_pass_find_layout_from_source_list(
            pass, source_lists.entries[k]);

        if (layout)
          render_pass_layout_disable_mesh(layout, mesh);
      }

      render_pass_sync_drawn_layouts(pass);
    }
  }

  return RendererStatus_Success;
}

RendererStatus renderer_visibility_toggle_mesh_in_pipeline(
    Renderer *rd, const RenderPipelineType pipeline, Mesh *mesh) {

  if (mesh_ref_list_find(renderer_mesh_state(rd, RendererMeshStates_Hidden),
                         mesh, NULL)) {
    renderer_show_mesh_in_pipeline(rd, pipeline, mesh);
    return RendererStatus_MeshVisible;
  } else {
    renderer_hide_mesh_in_pipeline(rd, pipeline, mesh);
    return RendererStatus_MeshHidden;
  }

  return RendererStatus_Success;
}
