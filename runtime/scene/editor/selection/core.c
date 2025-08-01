#include "core.h"
#include "../../show.h"
#include "./callback_key.h"
#include "./callback_mouse.h"
#include "emscripten/em_types.h"

/**
   Initialize the selection functionality on the scene main camera, meaning
   when a mesh is clicked, it displays the transform gizmo.
 */
void scene_selection_init(Scene *scene) {

  // init selection list
  mesh_ref_list_create(&scene->pipelines[ScenePipeline_Fixed_Selection],
                       SCENE_MESH_LIST_DEFAULT_CAPACITY);

  // init selection mouse events
  scene_selection_init_mouse_events(scene);

  // init selection keyboard events
  scene_selection_init_key_events(scene);
}

/**
   To transform the selected meshes and gizmo we poll the mouse event and check
   if the selection pipeline has length.

   Basically our camera raycast/ html events are only used to:
     1. push/pop mesh from the selection array (on right click)
     2. update the gizmo transform active axis (on left click)

    We then constantly through the loop:
     1. check if the mouse is pressed

    According to those checkes we then transform the meshes.

 */
void scene_selection_draw_callback(void *data) {

  Scene *cast_scene = (Scene *)data;
  GizmoTransform *gizmo = &cast_scene->editor.gizmo.transform;
  MeshRefList *selection_list = &gizmo->cache.selection;
  // look-up transform callback
  gizmo_transform_callback transform_callback =
      gizmo->transform_callback[gizmo->mode];

  if (selection_list->length)
    transform_callback(gizmo, cast_scene->active_camera, &cast_scene->viewport);
}
