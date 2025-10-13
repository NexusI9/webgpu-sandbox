#include "highlight.h"

#include <stddef.h>
#include <stdint.h>

#include "backend/logger.h"
#include "backend/registry.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/editor.h"
#include "runtime/scene/renderer/core.h"
#include "runtime/scene/renderer/render_pass/visibility.h"
#include "runtime/shader/update.h"

/**
   Function triggered when meshes are selected.

   We hereby mark the meshes as highlighted by pushing their pointer to the
   Scene Selection List.

   We then enables the meshes in each render pass that uses this Scene Selection
   List as Source list.

   NOTE: To enable the outline effect we need to target 2 passes:
   1. The stencil pass (included in the Default pass): the drawn mesh using
   shader will just be used to write in the stencil.
   2. The outline pass (included in the Outline pass since it need to always be
   in front): read the stencil result from the previous pass and manipulate the
   stencil.

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

  SceneSelectionType target_type[2] = {
      SceneSelectionType_Mesh,
      SceneSelectionType_MeshShadow,
  };

  const color highlight_color = {1.0f, 0.0f, 0.0f, 1.0f};
  const color default_color = {0.0f, 0.0f, 0.0f, 1.0f};

  // enable mesh in each fixed selection of each pass (outline + stencil) in all
  // draw modes
  for (SceneRendererDrawMode i = 0; i < SCENE_RENDERER_DRAW_MODE_COUNT; i++) {

    RenderPassList *pass_list = &scene->renderer.draw.render_pass[i];

    switch ((1 << i)) {

      // update line effect
    case SceneRendererDrawMode_Boundbox:
    case SceneRendererDrawMode_Wireframe:

      // disable all
      for (SceneSelectionType i = 0; i < 2; i++) {
        for (size_t j = 0; j < scene->editor.selection.filters[i].meshes.length;
             j++) {
          Mesh *mesh = scene->editor.selection.filters[i].meshes.entries[j];
          shader_update_uniform_data(mesh_shader(mesh, MeshShader_Wireframe), 1,
                                     0, (void *)&default_color);
        }
      }

      // enable selected
      for (size_t k = 0; k < selection->length; k++) {
        Mesh *mesh = selection->entries[k].mesh;
        shader_update_uniform_data(mesh_shader(mesh, MeshShader_Wireframe), 1,
                                   0, (void *)&highlight_color);
      }

      break;

      // update outline effect
    case SceneRendererDrawMode_Solid:
    case SceneRendererDrawMode_Texture:

      for (ScenePass j = 0; j < 2; j++) {
        RenderPass *pass = &pass_list->passes[target_pass[j]];

        RenderPassDrawLayout *layout =
            render_pass_find_layout_from_source_list(pass, selection_list);

        if (layout) {

          render_pass_layout_disable_all_mesh(layout);

          for (size_t k = 0; k < selection->length; k++) {
            Mesh *mesh = selection->entries[k].mesh;
            render_pass_layout_enable_mesh(layout, mesh);
          }
        }
      }
      break;
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
    
    const RegEntry *reg_obj =
        reg_lookup(list->entries[i].targets[SSOTargetID_SEM]);

    SceneEditorMesh *sem = (SceneEditorMesh *)reg_obj->ptr;
    
    if (sem->select_callback)
      sem->select_callback(&(SEMHighlightCallback){sem});
  }
}
