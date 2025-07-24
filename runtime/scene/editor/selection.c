#include "selection.h"
#include "../show.h"

static inline void scene_selection_add(Scene *, Mesh *);
static inline void scene_selection_remove(Scene *, Mesh *);
static inline void scene_selection_average_position(Scene *, vec3 *);

/**
   Callback called during the scene main camera raycast mouse click.
   Define the logic for the selection process such as:
   - Adding / Removing meshes from the selection pipeline
   - Showing / Hidding the transform gizmo based on hit length
 */
void scene_selection_raycast_right_click_callback(
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
          mesh_reference_list_find(selection_list, hit->mesh);

      if (already_selected == NULL) {
        scene_selection_add(scene, hit->mesh);
      } else {
        scene_selection_remove(scene, hit->mesh);
      }

    }
    // right click : add to selection
    else if (mouseEvent->button == 2) {
      // clear selection and add new one
      mesh_reference_list_empty(selection_list);
      scene_selection_add(scene, hit->mesh);
    }
  }

  // handle gizmo
  if (selection_list->length > 0) {

    // get average position
    vec3 position;
    scene_selection_average_position(scene, &position);
    gizmo_transform_translate(gizmo, position);

    scene_show_mesh_reference_list(scene, gizmo->active_handle,
                                   ScenePipeline_Fixed);
  } else {
    // hide from the scene
    scene_hide_mesh_reference_list(scene, gizmo->active_handle,
                                   ScenePipeline_Fixed);
  }
}

/**
   Left click raycast callback.
   Check if one of the gizmo is clicked.
 */
void scene_selection_raycast_mouse_move_callback(
    CameraRaycastCallback *cast_data, const EmscriptenMouseEvent *mouseEvent,
    void *user_data) {

  SceneSelectionCallbackData *cast_user_data =
      (SceneSelectionCallbackData *)user_data;

  // cannot check mouse down in mouseEvent so poll the global input mouse state
  if (g_input.mouse.state == InputMouseState_Down) {
    printf("move\n");
  }
}

/**
   Initialize the selection functionality on the scene main camera, meaning when
   a mesh is clicked, it displays the transform gizmo.
 */
void scene_selection_init(Scene *scene) {

  // init selection list
  mesh_reference_list_create(&scene->pipelines[ScenePipeline_Selection],
                             SCENE_MESH_LIST_DEFAULT_CAPACITY);

  // cache selection exclude layer (ex: grid...)
  SceneLayer *exclude_layer =
      scene_layer_set_find(&scene->layers, SCENE_LAYER_GIZMO_UNSELECTABLE);

  // right click raycast on scene main camera (to select meshes)
  camera_raycast_mouse_click(
      scene->active_camera,
      &(CameraRaycastDescriptor){
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
          .callback = scene_selection_raycast_right_click_callback,
          .data = (void *)&(SceneSelectionCallbackData){.scene = scene},
          .size = sizeof(SceneSelectionCallbackData),
      });

  // left click raycast on scene main camera (to select gizmo transform)
  camera_raycast_mouse_hover(
      scene->active_camera,
      &(CameraRaycastDescriptor){
          .include =
              {
                  .lists =
                      (MeshRefList *[]){&scene->pipelines[ScenePipeline_Fixed]},
                  .length = 0,
              },
          .exclude =
              {
                  .lists = (MeshRefList *[]){&exclude_layer->meshes},
                  .length = 0,
              },
          .viewport = &scene->viewport,
          .callback = scene_selection_raycast_mouse_move_callback,
          .data = (void *)&(SceneSelectionCallbackData){.scene = scene},
          .size = sizeof(SceneSelectionCallbackData),
      });
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
  if (mesh_reference_list_find(&scene->pipelines[ScenePipeline_Selection],
                               mesh) == NULL)
    mesh_reference_list_insert(&scene->pipelines[ScenePipeline_Selection],
                               mesh);
}

/**
   Remove mesh from the selection.
 */
void scene_selection_remove(Scene *scene, Mesh *mesh) {

  mesh_reference_list_remove(&scene->pipelines[ScenePipeline_Selection], mesh);
}
