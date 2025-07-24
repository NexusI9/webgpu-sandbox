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
  MeshRefListArray include;
  MeshRefListArray exclude;

  // on move attribtues
  camera_raycast_callback callback;
  const EmscriptenMouseEvent *em_mouse_event;
  void *data;
  size_t size;
} CameraRaycastCheckBoundsDescriptor;

static void
camera_raycast_check_bounds(const CameraRaycastCheckBoundsDescriptor *);

static inline bool camera_raycast_is_excluded(const MeshRefListArray *array,
                                              Mesh *mesh) {
  bool is_excluded = false;
  for (size_t i = 0; i < array->length; i++)
    for (size_t j = 0; j < array->lists[i]->length; j++)
      if (mesh == array->lists[i]->entries[j])
        is_excluded = true;

  return is_excluded;
}

/**
   Traverse the meshes ref lists and check if the
 */
void camera_raycast_check_bounds(
    const CameraRaycastCheckBoundsDescriptor *desc) {

  Raycast ray;
  CameraRaycastHitList *hits = desc->hits;

  // cast from camera pov
  desc->cast_method(&ray, desc->camera, desc->viewport);

  // clear hit list
  camera_raycast_hit_list_empty(desc->hits);

  // go though each meshes of each ref lists and check bound
  for (size_t l = 0; l < desc->include.length; l++) {

    MeshRefList *ref_list = desc->include.lists[l];

    for (size_t m = 0; m < ref_list->length; m++) {
      Mesh *mesh = ref_list->entries[m];

      // check if mesh belongs in exclude list
      if (camera_raycast_is_excluded(&desc->exclude, mesh))
        continue;

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
      .data = cast_data->data,
      .include = cast_data->include,
      .exclude = cast_data->exclude,
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
      .data = cast_data->data,
      .em_mouse_event = mouseEvent,
      .include = cast_data->include,
      .exclude = cast_data->exclude,
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
  if (cast_data->include.length) {
    free(cast_data->include.lists);
    cast_data->include.lists = NULL;
  }

  if (cast_data->exclude.length) {
    free(cast_data->exclude.lists);
    cast_data->exclude.lists = NULL;
  }

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
