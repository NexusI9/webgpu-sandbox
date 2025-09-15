#include "callback.h"
#include "./method.h"
#include "./utils.h"
#include "core.h"
#include "hit_list.h"

#include "../utils/system.h"

/**
   ▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖     ▗▄▄▖ ▗▄▖ ▗▖   ▗▖   ▗▄▄▖  ▗▄▖  ▗▄▄▖▗▖ ▗▖ ▗▄▄▖
   ▐▌   ▐▌  ▐▌▐▌   ▐▛▚▖▐▌  █      ▐▌   ▐▌ ▐▌▐▌   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌▗▞▘▐▌
   ▐▛▀▀▘▐▌  ▐▌▐▛▀▀▘▐▌ ▝▜▌  █      ▐▌   ▐▛▀▜▌▐▌   ▐▌   ▐▛▀▚▖▐▛▀▜▌▐▌   ▐▛▚▖  ▝▀▚▖
   ▐▙▄▄▖ ▝▚▞▘ ▐▙▄▄▖▐▌  ▐▌  █      ▝▚▄▄▖▐▌ ▐▌▐▙▄▄▖▐▙▄▄▖▐▙▄▞▘▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌▗▄▄▞▘

 */

typedef struct {
  camera_raycast_cast_method cast_method;
  const EmscriptenMouseEvent *em_mouse_event;
} CameraRaycastCheckBoundsDescriptor;

static void
camera_raycast_check_bounds(CameraRaycastCallbackData *,
                            const CameraRaycastCheckBoundsDescriptor *);

/**
   Traverse the meshes ref lists and check if the
 */
void camera_raycast_check_bounds(
    CameraRaycastCallbackData *cam_desc,
    const CameraRaycastCheckBoundsDescriptor *bound_desc) {

  Raycast ray;
  CameraRaycastHitList *hits = cam_desc->hits;

  // cast from camera pov
  bound_desc->cast_method(&ray, cam_desc->camera, cam_desc->viewport);

  // clear hit list
  camera_raycast_hit_list_empty(cam_desc->hits);

  // go though each meshes of each ref lists and check bound
  for (size_t l = 0; l < cam_desc->include.length; l++) {

    MeshRefList *ref_list = cam_desc->include.lists[l];

    // printf("include length: %lu\n", ref_list->length);
    for (size_t m = 0; m < ref_list->length; m++) {
      Mesh *mesh = ref_list->entries[m];

      // check if mesh belongs in exclude list
      if (camera_raycast_is_excluded(&cam_desc->exclude, mesh))
        continue;

      AABB boundbox = mesh->topology.boundbox.world;

      if (cam_desc->space == CameraRaycastSpace_ScreenSpace)
        camera_raycast_screen_space(cam_desc->camera, mesh,
                                    cam_desc->screen_space_size, &boundbox);

      bool hit = false;

      switch (cam_desc->bound) {
      case CameraRaycastBound_AABB:
        hit = raycast_hit_aabb(&ray, &boundbox, &ray.distance);
        break;

      case CameraRaycastBound_OBB:
        hit = raycast_hit_obb(&ray, &mesh->topology.boundbox.local, mesh->model,
                              &ray.distance);
        break;
      }

      // check if raycast within mesh bound
      // add mesh pointer to temp ref list and sort by hit distance
      // (closer mesh first)
      if (hit && hits->length < hits->capacity) {

        // add mesh and distance to hit list
        hits->entries[hits->length].mesh = mesh;
        hits->entries[hits->length].distance = ray.distance;

        // update list length
        hits->length++;

        // sort new entry
        camera_raycast_hit_list_sort(cam_desc->hits);
      }
    }
  }

  // dispatch to callback if hits
  cam_desc->callback(
      &(CameraRaycastCallback){
          .raycast = &ray,
          .hits = hits,
          .last_hit = &cam_desc->last_hit,
      },
      bound_desc->em_mouse_event, cam_desc->data);

  // update last first hit
  cam_desc->last_hit = hits->entries[0];
};

bool camera_raycast_event_callback_center(
    int eventType, const EmscriptenMouseEvent *mouseEvent, void *data) {

  // select cast method
  camera_raycast_cast_method method = camera_raycast_cast_method_center;

  // call common checker
  camera_raycast_check_bounds((CameraRaycastCallbackData *)data,
                              &(CameraRaycastCheckBoundsDescriptor){
                                  .cast_method = method,
                                  .em_mouse_event = mouseEvent,
                              });

  return EM_FALSE;
}

bool camera_raycast_event_callback_mouse(int eventType,
                                         const EmscriptenMouseEvent *mouseEvent,
                                         void *data) {

  // select cast method
  camera_raycast_cast_method method = camera_raycast_cast_method_mouse;

  // call common checker
  camera_raycast_check_bounds((CameraRaycastCallbackData *)data,
                              &(CameraRaycastCheckBoundsDescriptor){
                                  .cast_method = method,
                                  .em_mouse_event = mouseEvent,
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
