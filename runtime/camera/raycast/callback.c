#include "callback.h"
#include "./method.h"
#include "hit_list.h"

/**
   ▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖     ▗▄▄▖ ▗▄▖ ▗▖   ▗▖   ▗▄▄▖  ▗▄▖  ▗▄▄▖▗▖ ▗▖ ▗▄▄▖
   ▐▌   ▐▌  ▐▌▐▌   ▐▛▚▖▐▌  █      ▐▌   ▐▌ ▐▌▐▌   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌▗▞▘▐▌
   ▐▛▀▀▘▐▌  ▐▌▐▛▀▀▘▐▌ ▝▜▌  █      ▐▌   ▐▛▀▜▌▐▌   ▐▌   ▐▛▀▚▖▐▛▀▜▌▐▌   ▐▛▚▖  ▝▀▚▖
   ▐▙▄▄▖ ▝▚▞▘ ▐▙▄▄▖▐▌  ▐▌  █      ▝▚▄▄▖▐▌ ▐▌▐▙▄▄▖▐▙▄▄▖▐▙▄▞▘▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌▗▄▄▞▘

 */

typedef struct {

  // raycast attribute and cast method (from center or mouse position)
  Camera *camera;
  Viewport *viewport;
  camera_raycast_cast_method cast_method;
  CameraRaycastHitList *hits;

  // mesh list raycast is tested against
  MeshRefList **mesh_lists;
  size_t length;

  // on move attribtues
  camera_raycast_callback callback;
  const EmscriptenMouseEvent *em_mouse_event;
  void *data;
  size_t size;
} CameraRaycastCheckBoundsDescriptor;

static void
camera_raycast_check_bounds(const CameraRaycastCheckBoundsDescriptor *);

/**
   Traverse the meshes ref lists and check if the
 */
void camera_raycast_check_bounds(
    const CameraRaycastCheckBoundsDescriptor *desc) {

  Raycast ray;
  CameraRaycastHitList *hits = desc->hits;

  // cast from camera pov
  desc->cast_method(&ray, desc->camera, desc->viewport);

  AABB box;
  glm_vec3_copy((vec3){-10.0f, -10.0f, -10.0f}, box.min);
  glm_vec3_copy((vec3){10.0f, 10.0f, 10.0f}, box.max);

  // clear hit list
  camera_raycast_hit_list_empty(desc->hits);

  // go though each meshes of each ref lists and check bound
  for (size_t l = 0; l < desc->length; l++) {

    MeshRefList *ref_list = desc->mesh_lists[l];

    for (size_t m = 0; m < ref_list->length; m++) {
      Mesh *mesh = ref_list->entries[m];

      // check if raycast within mesh bound
      // add mesh pointer to temp ref list and sort by hit distance (closer
      // mesh first)
      if (raycast_hit_aabb(&ray, &mesh->topology.boundbox.bound,
                           &ray.distance) &&
          hits->length < hits->capacity) {

        // add mesh and distance to hit list
        hits->entries[hits->length].mesh = mesh;
        hits->entries[hits->length].distance = ray.distance;

        // update list length
        hits->length++;

        // sort new entry
        camera_raycast_hit_list_sort(desc->hits);
      }
    }
  }

  // dispatch to callback
  desc->callback(
      &(CameraRaycastCallback){
          .raycast = &ray,
          .hits = hits,
      },
      desc->em_mouse_event, desc->data);
};

bool camera_raycast_event_callback_center(
    int eventType, const EmscriptenMouseEvent *mouseEvent, void *data) {

  // convert data
  CameraRaycastCallbackData *cast_data = (CameraRaycastCallbackData *)data;

  // select cast method
  camera_raycast_cast_method method = camera_raycast_cast_method_center;

  // call common checker
  camera_raycast_check_bounds(&(CameraRaycastCheckBoundsDescriptor){
      .camera = cast_data->camera,
      .viewport = cast_data->viewport,
      .cast_method = method,
      .callback = cast_data->callback,
      .em_mouse_event = mouseEvent,
      .length = cast_data->length,
      .data = cast_data->data,
      .mesh_lists = cast_data->mesh_lists,
      .hits = cast_data->hits,
  });

  return EM_FALSE;
}

bool camera_raycast_event_callback_mouse(int eventType,
                                         const EmscriptenMouseEvent *mouseEvent,
                                         void *data) {

  // convert data
  CameraRaycastCallbackData *cast_data = (CameraRaycastCallbackData *)data;

  // select cast method
  camera_raycast_cast_method method = camera_raycast_cast_method_mouse;

  // call common checker
  camera_raycast_check_bounds(&(CameraRaycastCheckBoundsDescriptor){
      .camera = cast_data->camera,
      .viewport = cast_data->viewport,
      .cast_method = method,
      .callback = cast_data->callback,
      .length = cast_data->length,
      .data = cast_data->data,
      .mesh_lists = cast_data->mesh_lists,
      .hits = cast_data->hits,
  });

  return EM_FALSE;
}

/**
   Common destructor for mouse events. Will be called when destroying the
   camera. Since we need to allocate camera event data on the heap (mesh
   reference list) We need to make sure to deallocate it after destroying the
   camera.
 */
bool camera_raycast_event_destructor(void *data) {

  // convert data
  CameraRaycastCallbackData *cast_data = (CameraRaycastCallbackData *)data;

  // free mesh reference lists
  free(cast_data->mesh_lists);
  cast_data->mesh_lists = NULL;

  // free raycast hits list
  free(cast_data->hits->entries);
  cast_data->hits->entries = NULL;

  free(cast_data->hits);
  cast_data->hits = NULL;

  // free user data (optional)
  if (cast_data->data) {
    free(cast_data->data);
    cast_data->data = NULL;
  }

  return EM_FALSE;
}
