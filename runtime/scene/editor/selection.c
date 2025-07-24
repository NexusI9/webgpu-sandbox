#include "selection.h"
#include "../show.h"
#include "emscripten/em_types.h"

static inline void scene_selection_add(Scene *, Mesh *);
static inline void scene_selection_remove(Scene *, Mesh *);
static inline void scene_selection_average_position(Scene *, vec3 *);

/**
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
  MeshRefList *selection_list = &scene->pipelines[ScenePipeline_Selection];
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;

  // early return if no hits
  if (cast_data->hits->length == 0)
    return;

  // else retrieve first hit only (closest to camera)
  CameraRaycastHit *hit = &cast_data->hits->entries[0];

  // add hit to selection pipeline
  if (hit) {
    // cap + right click : remove selection if exist, add if not
    if (mouseEvent->shiftKey && mouseEvent->button == 2) {

      Mesh *already_selected =
          mesh_ref_list_find(selection_list, hit->mesh);

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
  }

  // handle gizmo
  if (selection_list->length > 0) {

    // get average position
    vec3 position; 
    scene_selection_average_position(scene, &position);
    gizmo_transform_translate(gizmo, position);

    scene_show_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                                   ScenePipeline_Fixed);
  } else {
    // hide from the scene
    scene_hide_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                                   ScenePipeline_Fixed);
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
    // assign axis based of hit pointer index (0 = X, 1 = Y, 2 = Z)
    gizmo_transform_set_axis_from_mesh(gizmo, hit);
    // set active handle from current mode
    gizmo_transform_set_active(gizmo);
  }
}

/**
   Initialize the selection functionality on the scene main camera, meaning
   when a mesh is clicked, it displays the transform gizmo.
 */
void scene_selection_init(Scene *scene) {

  // init selection list
  mesh_ref_list_create(&scene->pipelines[ScenePipeline_Selection],
                             SCENE_MESH_LIST_DEFAULT_CAPACITY);

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
                          &scene->pipelines[ScenePipeline_Lit],
                          &scene->pipelines[ScenePipeline_Unlit],
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
   To transform the selected meshes and gizmo we poll the mouse event and check
   if the selection pipeline has length.

   Basically our camera raycast/ html events are only used to:
     1. push/pop mesh from the selection array (on right click)
     2. update the gizmo transform active axis (on left click)

    We then constantly through the loop:
     1. check if the mouse is pressed
     2. check the selection pipeline length
     3. check which axis is good

    According to those checkes we then transform the meshes.

 */
static int l = 0;
void scene_selection_draw_callback(void *data) {
  
  Scene *cast_scene = (Scene *)data;
  MeshRefList *selection_list = &cast_scene->pipelines[ScenePipeline_Selection];
  GizmoTransform *gizmo = &cast_scene->editor.gizmo.transform;

  if (g_input.mouse.state == InputMouseState_Down &&
      gizmo->active_handle != NULL && selection_list->length > 0)
    // look-up transform callback
    gizmo->transform_callback[gizmo->mode](gizmo, selection_list, cast_scene->active_camera);
}

/**
   Get the selection average position (used to translate the gizmo)
 */
void scene_selection_average_position(Scene *scene, vec3 *dest) {

  MeshRefList *selection = &scene->pipelines[ScenePipeline_Selection];

  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, *dest);

  if (selection->length == 0)
    return;

  for (size_t i = 0; i < selection->length; i++)
    glm_vec3_add(selection->entries[i]->position, *dest, *dest);

  glm_vec3_scale(*dest, 1.0f / selection->length, *dest);
}

/**
   Add mesh to the selection list
 */
void scene_selection_add(Scene *scene, Mesh *mesh) {

  // only add if mesh not already exists
  if (mesh_ref_list_find(&scene->pipelines[ScenePipeline_Selection],
                               mesh) == NULL)
    mesh_ref_list_insert(&scene->pipelines[ScenePipeline_Selection],
                               mesh);
}

/**
   Remove mesh from the selection.
 */
void scene_selection_remove(Scene *scene, Mesh *mesh) {

  mesh_ref_list_remove(&scene->pipelines[ScenePipeline_Selection], mesh);
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
