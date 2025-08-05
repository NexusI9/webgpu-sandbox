#include "mouse.h"
#include "../../../show.h"
#include "../core.h"
#include "../filter.h"
#include "../selection.h"
#include "../utils.h"
#include <stdint.h>

void scene_selection_init_mouse_events(Scene *scene) {

  /**
      ===================== ADD SELECTION RELATED EVENTS ===================

     1. Add a right click raycast: push/pop meshes from the selection pipeline.

     2. Add a left click raycast: on gizmo transform only to define selected
     axis.

     3. Add a draw callback: to poll mouse events and loop through selection to
     apply transform.

     4. Add a html event on mouse up

   */

  // cache selection exclude layer (ex: grid...)
  SceneLayer *exclude_layer =
      scene_layer_set_find(&scene->layers, SCENE_LAYER_UNSELECTABLE);

  SceneSelection *scene_selection = &scene->editor.selection;

  // right click raycast on scene main camera (to select meshes)
  camera_raycast(
      scene->active_camera,
      &(CameraRaycastDescriptor){
          .target = CameraRaycastTarget_MousePosition,
          .event = CameraRaycastEvent_MouseDown,
          .space = CameraRaycastSpace_WorldSpace,
          .viewport = &scene->viewport,
          .callback = scene_selection_raycast_mesh_callback,
          .data = (void *)&(SceneSelectionCallbackData){.scene = scene},
          .size = sizeof(SceneSelectionCallbackData),
          .include =
              {
                  .lists =
                      (MeshRefList *[]){
                          &scene_selection->filters[0]
                               .meshes[SceneSelectionState_Default],
                          &scene_selection->filters[1]
                               .meshes[SceneSelectionState_Default],
                      },
                  .length = SCENE_SELECTION_TYPE_COUNT,
              },
          .exclude = {0},
      });

  // left click raycast on scene main camera (to select gizmo transform)
  SceneLayer *gizmo_layer =
      scene_layer_set_find(&scene->layers, SCENE_LAYER_GIZMO_TRANSFORM);

  camera_raycast(
      scene->active_camera,
      &(CameraRaycastDescriptor){
          .target = CameraRaycastTarget_MousePosition,
          .event = CameraRaycastEvent_MouseDown,
          .space = CameraRaycastSpace_ScreenSpace, // use scree-space since
                                                   // gizmo have fixed scale
          .screen_space_size = GIZMO_TRANSFORM_SIZE,
          .include =
              {
                  .lists = (MeshRefList *[]){&gizmo_layer->meshes},
                  .length = 1,
              },
          .exclude = {0},
          .viewport = &scene->viewport,
          .callback = scene_selection_raycast_gizmo_callback,
          .data = (void *)&(SceneSelectionCallbackData){.scene = scene},
          .size = sizeof(SceneSelectionCallbackData),
      });

  // add draw callback
  scene_renderer_add_draw_callback(&scene->renderer,
                                   scene_selection_draw_callback, scene);

  // add mouse up / reset callback
  html_event_add_mouse_up(&(HTMLEventMouse){
      .data = (void *)scene,
      .size = 0, // set to 0 so no heap allocation (and use same data pointer)
      .owner = scene->id,
      .callback = scene_selection_reset_callback,
      .destructor = NULL,
  });
}

/**
   ▗▖  ▗▖ ▗▄▖ ▗▖ ▗▖ ▗▄▄▖▗▄▄▄▖
   ▐▛▚▞▜▌▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌
   ▐▌  ▐▌▐▌ ▐▌▐▌ ▐▌ ▝▀▚▖▐▛▀▀▘
   ▐▌  ▐▌▝▚▄▞▘▝▚▄▞▘▗▄▄▞▘▐▙▄▄▖

    ▗▄▄▖ ▗▄▖ ▗▖   ▗▖   ▗▄▄▖  ▗▄▖  ▗▄▄▖▗▖ ▗▖ ▗▄▄▖
   ▐▌   ▐▌ ▐▌▐▌   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌▗▞▘▐▌
   ▐▌   ▐▛▀▜▌▐▌   ▐▌   ▐▛▀▚▖▐▛▀▜▌▐▌   ▐▛▚▖  ▝▀▚▖
   ▝▚▄▄▖▐▌ ▐▌▐▙▄▄▖▐▙▄▄▖▐▙▄▞▘▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌▗▄▄▞▘


   Callback called during the scene main camera raycast mouse click.
   Define the logic for the selection process such as:
   - Adding / Removing meshes from the selection pipeline
   - Showing / Hidding the transform gizmo based on hit length

    1. Manipulate each method (mesh/ shader) source list
    2. If method has a destination then transfert source -> destination
    3. Merge both method source list to gizmo selection

     Mesh based highlight                 Shader based highlight
      .---------------.                      .---------------.
      |  Source list  |                      |  Source list  |
      '-------.-------'                      '-------.-------'
              |                                      |
      [[ Push / Pop  ]] ---------.---------- [[ Push / Pop  ]]
              |                  |                   |
          copy to                |           .-------'-------.
              |                  |           |    Update     |
      .-------'------.           |           | Uniform flag  |
      | Destination  |           |           '---------------'
      |  (pipeline)  |           |
      '------.-------'           |
             |                   |
      .--------------.           |
      | Render Pass  |           |
      | (Highlight)  |	         |
      '--------------'           |
                                 |
                                 |
                                 |
                      Gizmo Selection list (merge)
                                 |
              .-- array ---------|--------------------.
              | Mesh Source List + Shader Source List |
              '------------------|--------------------'
                                 |
              .------------------'--------------------.
              |   --------------------------------.   |
              |  ▲     Gizmo Transform Loop       ▼   |
              |  '--------------------------------    |
              '---------------------------------------'

 */
void scene_selection_raycast_mesh_callback(
    CameraRaycastCallback *cast_data, const EmscriptenMouseEvent *mouseEvent,
    void *user_data) {

  SceneSelectionCallbackData *cast_user_data =
      (SceneSelectionCallbackData *)user_data;

  Scene *scene = cast_user_data->scene;
  SceneSelection *selection = &scene->editor.selection;
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;

  // else retrieve first hit only (closest to camera)
  CameraRaycastHit *hit = &cast_data->hits->entries[0];

  // add hit to selection pipeline
  if (cast_data->hits->length > 0 && hit) {

    // get the tarfet selectionfilter to dispatch the hit mesh in the right
    // "corridor" (mesh or shader)
    SceneSelectionFilter *target_filter =
        scene_selection_filter_find_mesh(selection, hit->mesh);

    if (target_filter != NULL) {

      MeshRefList *filter_selection =
          &target_filter->meshes[SceneSelectionState_Selected];

      // cap + right click : remove selection if exist, add if not
      if (mouseEvent->shiftKey && mouseEvent->button == 2 &&
          scene_selection_filter_set_active(target_filter, hit->mesh) ==
              SceneSelectionFilterStatus_MeshAlreadySelected)

        scene_selection_filter_set_inactive(target_filter, hit->mesh);

      // right click : add to selection
      else if (mouseEvent->button == 2) {
        // clear selection and add new one
        scene_selection_empty(selection);
        scene_selection_filter_set_active(target_filter, hit->mesh);
      }

      // transfert source to destination
      if (target_filter->highlight_callback)
        target_filter->highlight_callback(filter_selection, scene);
    }
  } else {
    // empty selection
    scene_selection_empty(selection);
  }

  // handle gizmo
  if (scene_selection_length(&scene->editor.selection) > 0) {
    // get average position
    scene_gizmo_transform_pos_to_selection(gizmo, &scene->editor.selection);
    scene_gizmo_transform_show(scene);
  } else {
    // hide from the scene
    scene_gizmo_transform_hide(scene);
  }
}

/**
   Left click raycast callback.
   Check if one of the gizmo is clicked and define the axis.
 */
void scene_selection_raycast_gizmo_callback(
    CameraRaycastCallback *cast_data, const EmscriptenMouseEvent *mouseEvent,
    void *user_data) {

  SceneSelectionCallbackData *cast_user_data =
      (SceneSelectionCallbackData *)user_data;

  Mesh *hit = cast_data->hits->entries[0].mesh;

  if (mouseEvent->button == 0 && hit) {

    Scene *scene = cast_user_data->scene;
    GizmoTransform *gizmo = &scene->editor.gizmo.transform;

    // map active axis from hit handle pointer
    gizmo_transform_set_axis_from_mesh(gizmo, hit);

    // cache scene selection initial attributes
    scene_selection_cache_initial_attributes(&scene->editor.selection,
                                             gizmo->mode);

    // set active handle from current mode and initialize offset
    gizmo_transform_set_active(gizmo, scene->active_camera, &scene->viewport);
  }
}
