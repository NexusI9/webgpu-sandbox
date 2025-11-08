#include "visibility_system.h"
#include "runtime/engine/core.h"
#include "runtime/systems/scene_system.h"

void visibility_system_show_mesh(Scene *scene, Renderer *renderer, Mesh *mesh) {

  renderer_show_mesh(renderer, mesh);
  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void visibility_system_hide_mesh(Scene *scene, Renderer *renderer, Mesh *mesh) {

  renderer_hide_mesh(renderer, mesh);
  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void visibility_system_yshow_mesh_ref_list(Scene *scene, Renderer *renderer,
                                           MeshRefList *list) {

  renderer_show_mesh_ref_list(renderer, list);
  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void visiblity_system_hide_mesh_ref_list(Scene *scene, Renderer *renderer,
                                         MeshRefList *list) {

  renderer_hide_mesh_ref_list(renderer, list);
  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void visibility_system_toggle_mesh_visibility(Scene *scene, Renderer *renderer,
                                              Mesh *mesh) {

  renderer_visibility_toggle_mesh(renderer, mesh);
  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void visibility_system_show_gizmo(Gizmo *gizmo, Renderer *rd) {

  for (int i = 0; i < RENDERER_DRAW_MODE_COUNT; i++) {
    RenderPassList *pass_list = &rd->mesh_pass[i];

    RenderPassDrawLayout *layout = render_pass_find_layout_from_source_list(
        &pass_list->passes[RendererMeshPass_Gizmo],
        renderer_pipeline(rd, RendererPipeline_Fixed_Front));

    if (layout)
      render_pass_layout_enable_mesh_ref_list(layout,
                                              &gizmo->handles[gizmo->mode]);
  }
}

void visibility_system_hide_gizmo(Gizmo *gizmo, Renderer *rd) {

  for (int i = 0; i < RENDERER_DRAW_MODE_COUNT; i++) {
    RenderPassList *pass_list = &rd->mesh_pass[i];

    RenderPassDrawLayout *layout = render_pass_find_layout_from_source_list(
        &pass_list->passes[RendererMeshPass_Gizmo],
        renderer_pipeline(rd, RendererPipeline_Fixed_Front));

    if (layout)
      render_pass_layout_disable_all_mesh(layout);
  }
}
