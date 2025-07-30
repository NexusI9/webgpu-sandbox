#include "./utils.h"


/**
   Add mesh to the selection list
 */
void scene_selection_add(Scene *scene, Mesh *mesh) {

  // only add if mesh not already exists
  if (mesh_ref_list_find(&scene->pipelines[ScenePipeline_Fixed_Selection],
                         mesh) == NULL)
    mesh_ref_list_insert(&scene->pipelines[ScenePipeline_Fixed_Selection],
                         mesh);
}

/**
   Remove mesh from the selection.
 */
void scene_selection_remove(Scene *scene, Mesh *mesh) {

  mesh_ref_list_remove(&scene->pipelines[ScenePipeline_Fixed_Selection], mesh);
}

/**
   Set the gizmo active handle to NULL which acts as a trigger.
   This wall the loop callback doesn't move the meshes anymore if the mouse is
   down again.
 */
bool scene_selection_reset_callback(int eventType,
                                    const EmscriptenMouseEvent *mouseEvent,
                                    void *userData) {

  GizmoTransform *gizmo = (GizmoTransform *)userData;
  gizmo_transform_clear_active(gizmo);

  return EM_FALSE;
}
