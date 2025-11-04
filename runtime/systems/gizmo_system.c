#include "gizmo_system.h"
#include "backend/renderer/core.h"

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
