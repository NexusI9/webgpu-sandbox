#include "mouse.h"
#include "../../../show.h"
#include "../core.h"
#include "../filter.h"
#include "../runtime/mesh/shader/shader.h"
#include "../selection.h"
#include "../utils.h"
#include "../utils/color.h"
#include <stddef.h>
#include <stdint.h>

static const struct {
  CameraRaycastEvent event;
  camera_raycast_callback callback;
} selection_gizmo_mouse_events[2] = {
    {
        .event = CameraRaycastEvent_MouseDown,
        .callback = scene_selection_raycast_gizmo_down_callback,
    },
    {
        .event = CameraRaycastEvent_MouseHover,
        .callback = scene_selection_raycast_gizmo_hover_callback,
    },
};

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
  SceneSelection *scene_selection = &scene->editor.selection;

  MeshRefList *scene_selection_config_lists[SCENE_SELECTION_TYPE_COUNT];
  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    scene_selection_config_lists[i] =
        &scene_selection->filters[i].meshes[SceneSelectionState_Default];

  /*

     ===== MESHES EVENTS =====

   */

  // right click raycast on scene main camera (to select meshes)
  camera_raycast(
      scene->camera,
      &(CameraRaycastDescriptor){
          .target = CameraRaycastTarget_MousePosition,
          .event = CameraRaycastEvent_MouseDown,
          .space = CameraRaycastSpace_WorldSpace,
          .bound = CameraRaycastBound_OBB,
          .viewport = &scene->viewport,
          .callback = scene_selection_raycast_mesh_callback,
          .data = (void *)&(SceneSelectionCallbackData){.scene = scene},
          .size = sizeof(SceneSelectionCallbackData),
          .include =
              {
                  .lists = scene_selection_config_lists,
                  .length = SCENE_SELECTION_TYPE_COUNT,
              },
          .exclude = {0},
      });

  /*

     ===== GIZMO EVENTS =====

   */

  // left click raycast on scene main camera (to select gizmo transform)
  SceneLayer *gizmo_layer =
      scene_layer_set_find(&scene->layers, SCENE_LAYER_GIZMO_TRANSFORM);

  // map selection gizmo mouse events
  for (uint8_t i = 0; i < 2; i++)
    camera_raycast(
        scene->camera,
        &(CameraRaycastDescriptor){
            .target = CameraRaycastTarget_MousePosition,
            .event = selection_gizmo_mouse_events[i].event,
            // use scree-space since gizmo have fixed scale
            .space = CameraRaycastSpace_ScreenSpace,
            .screen_space_size = GIZMO_TRANSFORM_SIZE, // Gizmo size
            .include =
                {
                    .lists = (MeshRefList *[]){&gizmo_layer->meshes},
                    .length = 1,
                },
            .exclude = {0},
            .viewport = &scene->viewport,
            .callback = selection_gizmo_mouse_events[i].callback,
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

  if (mouseEvent->button != 2)
    return;

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
              SceneSelectionFilterStatus_MeshAlreadySelected) {

        scene_selection_filter_set_inactive(target_filter, hit->mesh);

        for (size_t i = 0; i < hit->mesh->children.length; i++)
          scene_selection_filter_set_inactive(target_filter,
                                              hit->mesh->children.entries[i]);

        // right click : add to selection
      } else if (mouseEvent->button == 2) {
        // clear selection and add new one
        scene_selection_empty(selection);
        scene_selection_filter_set_active(target_filter, hit->mesh);

        for (size_t i = 0; i < hit->mesh->children.length; i++)
          scene_selection_filter_set_active(target_filter,
                                            hit->mesh->children.entries[i]);
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
    scene_gizmo_transform_pos_to_selection(gizmo, &scene->editor.selection,
                                           &scene->renderer.ssbo);
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
void scene_selection_raycast_gizmo_down_callback(
    CameraRaycastCallback *cast_data, const EmscriptenMouseEvent *mouseEvent,
    void *user_data) {

  SceneSelectionCallbackData *cast_user_data =
      (SceneSelectionCallbackData *)user_data;

  Mesh *hit = cast_data->hits->entries[0].mesh;

  if (mouseEvent->button == 0 && hit) {

    Scene *scene = cast_user_data->scene;
    GizmoTransform *gizmo = &scene->editor.gizmo.transform;

    // if init distance > 0, means the gizmo is already active (from the hotkeyh
    // as instance)
    if (gizmo->cache.init_distance == 0.0) {

      // map active axis from hit handle pointer
      gizmo_transform_set_axis_from_mesh(gizmo, hit);

      // cache scene selection initial attributes
      scene_selection_cache_initial_attributes(&scene->editor.selection,
                                               gizmo->mode);

      // set active handle from current mode and initialize offset
      gizmo_transform_set_active(gizmo, scene->active_camera, &scene->viewport);

    }
  }
}

/**
   Hover on gizmo raycast callback.
   Check if one of the gizmo is clicked and define the axis.
 */

void scene_selection_raycast_gizmo_hover_callback(
    CameraRaycastCallback *cast_data, const EmscriptenMouseEvent *mouseEvent,
    void *user_data) {

  SceneSelectionCallbackData *cast_user_data =
      (SceneSelectionCallbackData *)user_data;
  GizmoTransform *gizmo = &cast_user_data->scene->editor.gizmo.transform;

  // for (size_t i = 0; i < cast_data->hits->length; i++)
  CameraRaycastHit *hit = &cast_data->hits->entries[0];

  // update only once
  if (cast_data->last_hit->mesh != hit->mesh &&
      // if mouse is down >> lock
      g_input.mouse.state == InputMouseState_Up) {

    if (hit->mesh) {

      // reset colors
      gizmo_transform_reset_color_uniform(gizmo);

      // update hovered gizmo color
      shader_update_uniform_data(mesh_shader(hit->mesh, MeshShader_Fixed), 1, 0,
                                 COLOR_GIZMO_TRANSFORM_HOVER);

    } else {
      gizmo_transform_reset_color_uniform(gizmo);
    }
  }
}
