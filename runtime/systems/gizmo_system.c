#include "gizmo_system.h"
#include "backend/renderer/core.h"
#include "runtime/systems/scene_system.h"

// Create transform gizmos and add them to editor gizmo
void gizmo_system_init(Gizmo *gizmo, Scene *scene, Renderer *renderer) {

  gizmo_create(gizmo, &(GizmoCreateDescriptor){
                          .camera = scene->active_camera,
                          .viewport = &scene->viewport,
                      });

  for (size_t i = 0; i < GIZMO_MODE_COUNT; i++) {
    for (size_t j = 0; j < gizmo->handles[i].length; j++) {
      Mesh *mesh = gizmo->handles[i].entries[j];
      scene_system_add_mesh_pipeline(
          scene, renderer, mesh, RendererPipeline_Fixed_Front,
          SCENE_LAYER_GIZMO,
          SceneAddFlag_Hide | SceneAddFlag_Unselectable |
              SceneAddFlag_TreeHide);
    }
  }
}

void gizmo_system_show(Gizmo *gizmo, Renderer *rd) {

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

void gizmo_system_hide(Gizmo *gizmo, Renderer *rd) {

  for (int i = 0; i < RENDERER_DRAW_MODE_COUNT; i++) {
    RenderPassList *pass_list = &rd->mesh_pass[i];

    RenderPassDrawLayout *layout = render_pass_find_layout_from_source_list(
        &pass_list->passes[RendererMeshPass_Gizmo],
        renderer_pipeline(rd, RendererPipeline_Fixed_Front));

    if (layout)
      render_pass_layout_disable_all_mesh(layout);
  }
}
