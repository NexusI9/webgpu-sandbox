#include "core.h"
#include "hit_list.h"
#include <stdlib.h>
#include <string.h>

/**
    ▗▄▄▖ ▗▄▖ ▗▄▄▖ ▗▄▄▄▖
   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌
   ▐▌   ▐▌ ▐▌▐▛▀▚▖▐▛▀▀▘
   ▝▚▄▄▖▝▚▄▞▘▐▌ ▐▌▐▙▄▄▖

 */

static inline void camera_raycast_create_event(Camera *,
                                               const CameraRaycastDescriptor *,
                                               em_mouse_callback_func,
                                               html_event_mouse);

static inline MeshRefList **malloc_reflist(MeshRefList **, size_t);

/**
   Allocate mesh_list on the heap
 */
MeshRefList **malloc_ref_list(const MeshRefListArray *array) {

  MeshRefList **alloc_list = malloc(array->length * sizeof(MeshRefList *));
  if (alloc_list == NULL)
    return NULL;

  memcpy(alloc_list, array->lists, array->length * sizeof(MeshRefList *));

  return alloc_list;
}

/**
   Since the html event and the loop are not part of the same "timeline"
   We need to allocate the event data on the heap so it doesn't go out of scope.
   However we passed a destructor in the html_event listener to ensure the data
   won't stagnate once we remove the owner objects.
 */
void camera_raycast_create_event(Camera *cam,
                                 const CameraRaycastDescriptor *desc,
                                 em_mouse_callback_func em_callback,
                                 html_event_mouse html_event_callback) {

  // === ALLOCATE MESH REFERENCES ===
  MeshRefList **alloc_include = NULL;
  MeshRefList **alloc_exclude = NULL;

  if (desc->include.length > 0)
    alloc_include = malloc_ref_list(&desc->include);

  if (desc->exclude.length > 0)
    alloc_exclude = malloc_ref_list(&desc->exclude);

  if ((desc->include.length > 0 && alloc_include == NULL) ||
      (desc->exclude.length > 0 && alloc_exclude == NULL)) {
    VERBOSE_WARNING("Couldn't allocate raycast mesh ref list.\n");
    return;
  }

  // === ALLOCATE HIT LIST === (sorted from closest hit to further)
  CameraRaycastHitList *hits_list = malloc(sizeof(CameraRaycastHitList));
  if (hits_list == NULL || camera_raycast_hit_list_create(
                               hits_list, CAMERA_RAYCAST_HIT_LIST_MAX_HIT) !=
                               CAMERA_RAYCAST_HIT_LIST_SUCCESS) {
    VERBOSE_ERROR("Couldn't allocate camera raycast 'hit list'\n");
    return;
  }

  // === ALLOCATE USER DATA ===
  void *alloc_data = malloc(sizeof(desc->size));
  if (alloc_data == NULL) {
    VERBOSE_ERROR("Couldn't allocate camera raycast 'data'\n");
    return;
  }
  memcpy(alloc_data, desc->data, desc->size);

  // convert data (add camera and hit list)
  const CameraRaycastCallbackData data = {
      // cb attributes
      .callback = desc->callback,
      .data = alloc_data,
      .size = desc->size,

      // cast attributes
      .camera = cam,
      .viewport = desc->viewport,
      .hits = hits_list,

      // bound attributes
      .include =
          {
              .lists = alloc_include,
              .length = desc->include.length,
          },
      .exclude =
          {
              .lists = alloc_exclude,
              .length = desc->exclude.length,
          }

  };
 
  
  // add listener
  html_event_callback(&(HTMLEventMouse){
      .callback = em_callback,
      .destructor = camera_raycast_event_destructor,
      .data = (void *)&data,
      .size = sizeof(CameraRaycastCallbackData),
      .owner = cam->id,
  });
}

/**
    ▗▄▄▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▄▄▄▖▗▄▄▖
   ▐▌   ▐▌   ▐▛▚▖▐▌  █  ▐▌   ▐▌ ▐▌
   ▐▌   ▐▛▀▀▘▐▌ ▝▜▌  █  ▐▛▀▀▘▐▛▀▚▖
   ▝▚▄▄▖▐▙▄▄▖▐▌  ▐▌  █  ▐▙▄▄▖▐▌ ▐▌

   Link to the camera a raycast system with the center of screen as raycast
   target. Useful for Flying or orbit mode in which cursor is usually hidden.
 */
void camera_raycast_center_hover(Camera *cam,
                                 const CameraRaycastDescriptor *desc) {

  camera_raycast_create_event(cam, desc, camera_raycast_event_callback_center,
                              html_event_add_mouse_move);
}

void camera_raycast_center_click(Camera *cam,
                                 const CameraRaycastDescriptor *desc) {

  camera_raycast_create_event(cam, desc, camera_raycast_event_callback_center,
                              html_event_add_mouse_down);
}

/**
   ▗▖  ▗▖ ▗▄▖ ▗▖ ▗▖ ▗▄▄▖▗▄▄▄▖
   ▐▛▚▞▜▌▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌
   ▐▌  ▐▌▐▌ ▐▌▐▌ ▐▌ ▝▀▚▖▐▛▀▀▘
   ▐▌  ▐▌▝▚▄▞▘▝▚▄▞▘▗▄▄▞▘▐▙▄▄▖

   Link to the camera a raycast system with the mouse position as raycast
   target. Useful for Edit mode.
 */
void camera_raycast_mouse_hover(Camera *cam,
                                const CameraRaycastDescriptor *desc) {

  camera_raycast_create_event(cam, desc, camera_raycast_event_callback_mouse,
                              html_event_add_mouse_move);
}

void camera_raycast_mouse_click(Camera *cam,
                                const CameraRaycastDescriptor *desc) {

  camera_raycast_create_event(cam, desc, camera_raycast_event_callback_mouse,
                              html_event_add_mouse_down);
}
