#include "./utils.h"

#include <cglm/types.h>

#include "./core.h"
#include "backend/ssbo.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "runtime/scene/renderer/render_pass/core.h"
#include "runtime/scene/renderer/render_pass/visibility.h"
#include "runtime/scene/show.h"

void scene_gizmo_show(Scene *scene) {
  Gizmo *gizmo = &scene->editor.gizmo.transform;

  for (SceneRendererDrawMode i = 0; i < SCENE_RENDERER_DRAW_MODE_COUNT; i++) {
    RenderPassList *pass_list = &scene->renderer.draw.render_pass[i];

    RenderPassDrawLayout *layout = render_pass_find_layout_from_source_list(
        &pass_list->passes[ScenePass_Gizmo],
        scene_pipeline(scene, ScenePipeline_Fixed_Front));

    if (layout)
      render_pass_layout_enable_mesh_ref_list(layout,
                                              &gizmo->handles[gizmo->mode]);
  }
}

void scene_gizmo_hide(Scene *scene) {

  Gizmo *gizmo = &scene->editor.gizmo.transform;

  for (SceneRendererDrawMode i = 0; i < SCENE_RENDERER_DRAW_MODE_COUNT; i++) {
    RenderPassList *pass_list = &scene->renderer.draw.render_pass[i];
    
    RenderPassDrawLayout *layout = render_pass_find_layout_from_source_list(
        &pass_list->passes[ScenePass_Gizmo],
        scene_pipeline(scene, ScenePipeline_Fixed_Front));

    if (layout)
      render_pass_layout_disable_all_mesh(layout);
  }
}

/**
   Get the selection average position (used to translate the gizmo).
 */
void scene_gizmo_pos_to_selection(Gizmo *gizmo, SceneSelection *selection,
                                  SSBOManager *ssbo) {

  // get average position
  vec3 position;
  scene_selection_average_position(selection, &position);
  gizmo_set_position(gizmo, position);

  // update ssbo matrix buffer
  gizmo_update_ssbo(gizmo, ssbo);
}
