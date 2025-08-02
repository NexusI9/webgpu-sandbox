#include "callback_mouse.h"
#include "../../show.h"
#include "./selection.h"
#include "./utils.h"
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

  const uint8_t selection_rules_count = 2;

  SceneSelectionRuleSet *refs_list[2] = {
      // Mesh based rules
      &scene->editor.selection.mesh_based,
      // Shader based rules
      &scene->editor.selection.shader_based,
  };

  // right click raycast on scene main camera (to select meshes)
  for (size_t i = 0; i < selection_rules_count; i++) {
    camera_raycast(scene->active_camera,
                   &(CameraRaycastDescriptor){
                       .target = CameraRaycastTarget_MousePosition,
                       .event = CameraRaycastEvent_MouseDown,
                       .space = CameraRaycastSpace_WorldSpace,
                       .viewport = &scene->viewport,
                       .callback = scene_selection_raycast_mesh_callback,
                       .data =
                           (void *)&(SceneSelectionCallbackData){
                               .scene = scene,
                               .source = &refs_list[i]->source,
                               .destination = refs_list[i]->destination,
                           },
                       .size = sizeof(SceneSelectionCallbackData),
                       .include =
                           {
                               .lists = refs_list[i]->include.entries,
                               .length = refs_list[i]->include.length,
                           },
                       .exclude =
                           {
                               .lists = refs_list[i]->exclude.entries,
                               .length = refs_list[i]->exclude.length,
                           },
                   });
  }

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
  MeshRefList *source_list = cast_user_data->source;
  MeshRefList *destination_list = cast_user_data->destination;
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;

  // else retrieve first hit only (closest to camera)
  CameraRaycastHit *hit = &cast_data->hits->entries[0];

  // add hit to selection pipeline
  if (cast_data->hits->length > 0 && hit) {

    // cap + right click : remove selection if exist, add if not
    if (mouseEvent->shiftKey && mouseEvent->button == 2) {

      Mesh *already_selected = mesh_ref_list_find(source_list, hit->mesh);

      if (already_selected == NULL)
        scene_selection_add(source_list, hit->mesh);
      else
        mesh_ref_list_remove(source_list, hit->mesh);

    }
    // right click : add to selection
    else if (mouseEvent->button == 2) {
      // clear selection and add new one
      mesh_ref_list_empty(source_list);
      scene_selection_add(source_list, hit->mesh);
    }
  } else {
    // empty selection
    mesh_ref_list_empty(source_list);
    // hide from the scene
    scene_hide_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                             ScenePipeline_Fixed_Front);
  }

  // handle gizmo
  if (source_list->length > 0) {

    // get average position
    scene_gizmo_transform_pos_to_selection(gizmo, source_list);
    scene_show_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                             ScenePipeline_Fixed_Front);
  }

  // transfert source to destination
  if (destination_list) {
    mesh_ref_list_empty(destination_list);
    mesh_ref_list_transfert(source_list, destination_list, NULL);
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

    // set active handle from current mode and initialize offset
    gizmo_transform_set_active(
        gizmo,
        &(MeshRefListArray){
            .lists =
                (MeshRefList *[2]){
                    &scene->editor.selection.mesh_based.source,
                    &scene->editor.selection.shader_based.source,
                },
            .length = 2,
        },
        scene->active_camera, &scene->viewport);
  }
}
