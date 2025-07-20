#include "selection.h"

static void scene_selection_add(Scene *, Mesh *);
static void scene_selection_remove(Scene *, Mesh *);

void scene_selection_raycast_callback(CameraRaycastCallback *cast_data,
                                      const EmscriptenMouseEvent *mouseEvent,
                                      void *user_data) {

  SceneSelectionCallbackData *cast_user_data =
      (SceneSelectionCallbackData *)user_data;

  Scene *scene = cast_user_data->scene;
  SceneLayer *exclude_layer = cast_user_data->exclude_layer;

  // early return if no hits
  if (cast_data->hits->length == 0)
    return;

  // else retrieve first hit only (closest to camera)
  size_t index = 0;
  CameraRaycastHit *hit = &cast_data->hits->entries[index];

  // check if object is blacklisted (exclude layer)
  while (hit != NULL && exclude_layer != NULL &&
         scene_layer_find(exclude_layer, hit->mesh) != NULL &&
         index < cast_data->hits->length) {

    // skip to next hit mesh
    hit = &cast_data->hits->entries[++index];
  }

  if (hit) {

    // cap + right click : remove selection if exist, add if not
    if (mouseEvent->shiftKey && mouseEvent->button == 2) {

      Mesh *already_selected = mesh_reference_list_find(
          &scene->editor.pipelines.selection, hit->mesh);

      if (already_selected == NULL) {
        scene_selection_add(scene, hit->mesh);
      } else {
        scene_selection_remove(scene, hit->mesh);
      }

    }
    // right click : add to selection
    else if (mouseEvent->button == 2) {
      // clear selection and add new one
      mesh_reference_list_empty(&scene->editor.pipelines.selection);
      scene_selection_add(scene, hit->mesh);
    }
  }
}

/**
   Initialize the selection functionality on the scene main camera, meaning when
   a mesh is clicked, it displays the transform gizmo.
 */
void scene_selection_init(Scene *scene) {

  // init selection list
  mesh_reference_list_create(&scene->editor.pipelines.selection,
                             SCENE_MESH_LIST_DEFAULT_CAPACITY);

  // cache selection exclude layer (ex: grid...)
  SceneLayer *exclude_layer =
      scene_layer_set_find(&scene->layers, SCENE_LAYER_GIZMO_UNSELECTABLE);

  // raycast on scene main camera
  camera_raycast_mouse_click(scene->active_camera,
                             &(CameraRaycastDescriptor){
                                 .mesh_lists =
                                     (MeshRefList *[]){
                                         &scene->pipelines.lit,
                                         &scene->pipelines.unlit,
                                         &scene->pipelines.fixed,
                                     },
                                 .length = 3,
                                 .viewport = &scene->viewport,
                                 .callback = scene_selection_raycast_callback,
                                 .data =
                                     (void *)&(SceneSelectionCallbackData){
                                         .scene = scene,
                                         .exclude_layer = exclude_layer,
                                     },
                                 .size = sizeof(SceneSelectionCallbackData),
                             });
}

/**
   Add mesh to the selection list
 */
void scene_selection_add(Scene *scene, Mesh *mesh) {

  // only add if mesh not already exists
  if (mesh_reference_list_find(&scene->editor.pipelines.selection, mesh) ==
      NULL)
    mesh_reference_list_insert(&scene->editor.pipelines.selection, mesh);
}

/**
   Remove mesh from the selection.
 */
void scene_selection_remove(Scene *scene, Mesh *mesh) {

  mesh_reference_list_remove(&scene->editor.pipelines.selection, mesh);
}
