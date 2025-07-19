#include "selection.h"

static void scene_selection_add(Scene *, Mesh *);
static void scene_selection_remove(Scene *, Mesh *);

void scene_selection_raycast_callback(CameraRaycastCallback *cast_data,
                                      const EmscriptenMouseEvent *mouseEvent,
                                      void *user_data) {

  // early return if no hits
  if (cast_data->hits->length == 0)
    return;

  // else retrieve first hit only (closest to camera)
  CameraRaycastHit *hit = &cast_data->hits->entries[0];
  
  if (hit) {

    // cap + right click : remove selection
    if (mouseEvent->shiftKey && mouseEvent->button == 2) {
      printf("remove: %s\n", hit->mesh->name);
      return;
    }

    // right click : add to selection
    if (mouseEvent->button == 2) {
      printf("add %s \n", hit->mesh->name);
      return;
    }

  }
}

/**
   Initialize the selection functionality on the scene main camera, meaning when
   a mesh is clicked, it displays the transform gizmo.
 */
void scene_selection_init(Scene *scene) {

  // init selection list
  mesh_reference_list_create(&scene->selection,
                             SCENE_MESH_LIST_DEFAULT_CAPACITY);

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
                                 .data = NULL,
                             });
}

/**
   Add mesh to the selection list
 */
void scene_selection_add(Scene *scene, Mesh *mesh) {}

/**
   Remove mesh from the selection
 */
void scene_selection_remove(Scene *scene, Mesh *mesh) {}
