#include "mouse.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../core.h"
#include "../filter.h"
#include "../utils.h"
#include "backend/logger.h"
#include "runtime/camera/raycast/core.h"
#include "runtime/camera/raycast/hit_list.h"
#include "runtime/html_event/add.h"
#include "runtime/html_event/core.h"
#include "runtime/input/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "runtime/scene/layer.h"
#include "runtime/scene/renderer/core.h"
#include "runtime/shader/update.h"
#include "utils/color.h"

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
  for (SceneSelectionType i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    scene_selection_config_lists[i] = &scene_selection->filters[i].meshes;

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
      scene_layer_set_find(&scene->layers, SCENE_LAYER_GIZMO);

  // map selection gizmo mouse events
  for (uint8_t i = 0; i < 2; i++)
    camera_raycast(
        scene->camera,
        &(CameraRaycastDescriptor){
            .target = CameraRaycastTarget_MousePosition,
            .event = selection_gizmo_mouse_events[i].event,
            // use scree-space since gizmo have fixed scale
            .space = CameraRaycastSpace_ScreenSpace,
            .screen_space_size = GIZMO_SIZE, // Gizmo size
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

  scene_renderer_add_draw_callback(
      &scene->renderer, scene_selection_draw_callback, scene,
      SceneRendererDrawMode_Texture | SceneRendererDrawMode_Solid |
          SceneRendererDrawMode_Wireframe | SceneRendererDrawMode_Boundbox);

  // reset on mouse up
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
  Gizmo *gizmo = &scene->editor.gizmo.transform;

  // else retrieve first hit only (closest to camera)
  CameraRaycastHit *hit = &cast_data->hits->entries[0];

  if (mouseEvent->button != 2)
    return;

  if (cast_data->hits->length > 0 && hit)
    scene_selection_toggle_mesh(scene, hit->mesh);
  else {
    scene_selection_empty(selection);
    scene_gizmo_hide(scene);
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
    Gizmo *gizmo = &scene->editor.gizmo.transform;

    // if init distance > 0, means the gizmo is already active (from the hotkeyh
    // as instance)
    if (gizmo->cache.init_distance == 0.0) {

      // map active axis from hit handle pointer
      gizmo_set_axis_from_mesh(gizmo, hit);

      // cache scene selection initial attributes
      scene_selection_cache_initial_attributes(&scene->editor.selection,
                                               gizmo->mode);

      // set active handle from current mode and initialize offset
      gizmo_set_active(gizmo, scene->active_camera, &scene->viewport);
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

  Gizmo *gizmo = &cast_user_data->scene->editor.gizmo.transform;
  CameraRaycastHit *hit = NULL;

  // Since we recieve multiple hits (gizmo-mode agnostic) we need to filter down
  // and select the hit from the right gizmo_mode, else we may hover the rotate
  // gizmo being in the position mode.
  for (size_t i = 0; i < cast_data->hits->length; i++)
    if (mesh_ref_list_find(&gizmo->interactive_handles[gizmo->mode],
                           cast_data->hits->entries[i].mesh, NULL)) {
      hit = &cast_data->hits->entries[i];
      break;
    }

  if (hit == NULL) {
    if (cast_data->last_hit->mesh != NULL &&
        g_input.mouse.state == InputMouseState_Up)
      gizmo_reset_color_uniform(gizmo);
    return;
  }

  // update only once
  if (cast_data->last_hit->mesh != hit->mesh &&
      // if mouse is down >> lock
      g_input.mouse.state == InputMouseState_Up) {

    gizmo_reset_color_uniform(gizmo);
    // update hovered gizmo color
    shader_update_uniform_data(mesh_shader(hit->mesh, MeshShader_Fixed), 1, 0,
                               (void *)COLOR_GIZMO_HOVER,
                               ShaderUpdateFlag_None);
  } else {
  }
}
