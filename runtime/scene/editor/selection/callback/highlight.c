#include "highlight.h"

#include <stddef.h>
#include <stdint.h>

#include "backend/logger.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/editor.h"
#include "runtime/scene/renderer/render_pass/visibility.h"

/**
   Read the selection objects list on mesh selection
 */
void scene_selection_mesh_highlight(MeshRefList *meshes,
                                    SceneSelectionObjectList *selection,
                                    void *data) {

  Scene *scene = (Scene *)data;

  MeshRefList *selection_list =
      scene_pipeline(scene, ScenePipeline_Fixed_Selection);

  ScenePass target_pass[2] = {
      ScenePass_Default,
      ScenePass_Outline,
  };

  // enable mesh in each fixed selection of each pass (outline + stencil)
  for (uint8_t i = 0; i < 2; i++) {
    RenderPass *pass = &scene_renderer_active_pass_list(&scene->renderer)
                            ->passes[target_pass[i]];

    RenderPassDrawLayout *layout =
        render_pass_find_layout_from_source_list(pass, selection_list);

    if (layout) {
      render_pass_layout_disable_all_mesh(layout);
      for (size_t i = 0; i < selection->length; i++) {
        Mesh *mesh = selection->entries[i].mesh;
        render_pass_layout_enable_mesh(layout, mesh);
      }
    }
  }
};

/**
   For SEM Object we use a OOP approach (similar to the transform callback)
   where each SEM Mesh has its own transform and highlight callback.

   Since SEM are such polymorphic objects, it just easier and less messy to hook
   each mesh a transform and highlight callback.
 */
void scene_selection_sem_highlight(MeshRefList *meshes,
                                   SceneSelectionObjectList *list, void *data) {

  Scene *scene = (Scene *)data;

  // disable selected ones
  SceneEditorMeshListArray *sem_array = scene_editor_mesh_list(&scene->editor);
  for (size_t i = 0; i < sem_array->length; i++)
    for (size_t j = 0; j < sem_array->entries[i].length; j++) {
      SceneEditorMesh *sem = &sem_array->entries[i].entries[j];
      if (sem->deselect_callback)
        sem->deselect_callback(&(SEMHighlightCallback){sem});
    }

  // enable selected ones
  for (size_t i = 0; i < list->length; i++) {

    SceneEditorMesh *sem = (SceneEditorMesh *)list->entries[i].target;
    Mesh *mesh = list->entries[i].mesh;

    if (sem->select_callback)
      sem->select_callback(&(SEMHighlightCallback){sem});
  }
}
