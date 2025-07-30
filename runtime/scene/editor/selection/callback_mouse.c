#include "callback_mouse.h"
#include "./selection.h"
#include "../../show.h"
#include "./utils.h"

void scene_selection_init_mouse_events(Scene* scene) {

  /**
      ===================== ADD SELECTION RELATED EVENTS ===================

     1. Add a right click raycast: push/pop meshes from the selection pipeline.

     2. Add a left click raycast: on gizmo transform only to define selected
     axis.

     3. Add a draw callback: to poll mouse events and loop through selection to
     apply transform.

     4. Add a html event on mouse up: reset axis to -1

   */

  // cache selection exclude layer (ex: grid...)
  SceneLayer *exclude_layer =
      scene_layer_set_find(&scene->layers, SCENE_LAYER_GIZMO_UNSELECTABLE);

  // right click raycast on scene main camera (to select meshes)
  camera_raycast(
      scene->active_camera,
      &(CameraRaycastDescriptor){
          .target = CameraRaycastTarget_MousePosition,
          .event = CameraRaycastEvent_MouseDown,
          .space = CameraRaycastSpace_WorldSpace,
          .include =
              {
                  .lists =
                      (MeshRefList *[]){
                          &scene->pipelines[ScenePipeline_Dynamic_Lit],
                          &scene->pipelines[ScenePipeline_Dynamic_Unlit],
                          &scene->pipelines[ScenePipeline_Fixed],
                      },
                  .length = 3,
              },
          .exclude =
              {
                  .lists = (MeshRefList *[]){&exclude_layer->meshes},
                  .length = 1,
              },
          .viewport = &scene->viewport,
          .callback = scene_selection_raycast_mesh_callback,
          .data = (void *)&(SceneSelectionCallbackData){.scene = scene},
          .size = sizeof(SceneSelectionCallbackData),
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
      .data = (void *)&scene->editor.gizmo.transform,
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
 */
void scene_selection_raycast_mesh_callback(
    CameraRaycastCallback *cast_data, const EmscriptenMouseEvent *mouseEvent,
    void *user_data) {

  SceneSelectionCallbackData *cast_user_data =
      (SceneSelectionCallbackData *)user_data;

  Scene *scene = cast_user_data->scene;
  MeshRefList *selection_list =
      &scene->pipelines[ScenePipeline_Fixed_Selection];
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;

  // else retrieve first hit only (closest to camera)
  CameraRaycastHit *hit = &cast_data->hits->entries[0];

  // add hit to selection pipeline
  if (cast_data->hits->length > 0 && hit) {

    // cap + right click : remove selection if exist, add if not
    if (mouseEvent->shiftKey && mouseEvent->button == 2) {

      Mesh *already_selected = mesh_ref_list_find(selection_list, hit->mesh);

      if (already_selected == NULL) {
        scene_selection_add(scene, hit->mesh);
      } else {
        scene_selection_remove(scene, hit->mesh);
      }

    }
    // right click : add to selection
    else if (mouseEvent->button == 2) {
      // clear selection and add new one
      mesh_ref_list_empty(selection_list);
      scene_selection_add(scene, hit->mesh);
    }
  } else {
    // empty selection
    mesh_ref_list_empty(selection_list);
    // hide from the scene
    scene_hide_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                             ScenePipeline_Fixed_Front);
  }

  // handle gizmo
  if (selection_list->length > 0) {

    // get average position
    vec3 position;
    scene_selection_average_position(scene, &position);
    gizmo_transform_translate(gizmo, position);

    scene_show_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                             ScenePipeline_Fixed_Front);
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
    // set active handle from current mode and initialize offset
    gizmo_transform_set_active(gizmo, hit,
                               &scene->pipelines[ScenePipeline_Fixed_Selection],
                               scene->active_camera, &scene->viewport);
  }
}
