#include "highlight.h"

#include <stddef.h>
#include <stdint.h>

#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/scene/core.h"
#include "runtime/scene/renderer/render_pass/visibility.h"

/**
   Read the selection objects list on mesh selection
 */
void scene_selection_mesh_highlight(MeshRefList *meshes,
                                    SceneSelectionObjectList *selection,
                                    void *data) {

  Scene *scene = (Scene *)data;

  MeshRefList *ref_list = scene_pipeline(scene, ScenePipeline_Fixed_Selection);

  RenderPass *pass =
      &scene->renderer.draw.render_pass[scene->renderer.draw.mode]
           .passes[ScenePass_Selection];

  RenderPassDrawLayout *layout = render_pass_find_layout_from_source_list(
      pass, scene_pipeline(scene, ScenePipeline_Fixed_Selection));

  if (layout) {
    render_pass_layout_disable_all_mesh(layout);
    for (size_t i = 0; i < selection->length; i++) {
      Mesh *mesh = selection->entries[i].mesh;
      render_pass_layout_enable_mesh(layout, mesh);
    }
  }
};

void scene_selection_seo_highlight(MeshRefList *meshes,
                                   SceneSelectionObjectList *list, void *data) {
}
