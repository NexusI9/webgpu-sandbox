#include "selection.h"

static void scene_selection_add(Scene *, Mesh *);
static void scene_selection_remove(Scene *, Mesh *);

void scene_selection_raycast_callback(CameraRaycastCallback *cast_data,
                                      const EmscriptenMouseEvent *mouseEvent,
                                      void *user_data) {

  printf("hit mesh: \n");
  for (size_t i = 0; i < cast_data->hits->length; i++) {
    printf("name: %s\n", cast_data->hits->entries[i].mesh->name);
    printf("distance: %f\n", cast_data->hits->entries[i].distance);
    printf("-----\n");
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
