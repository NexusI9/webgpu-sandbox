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
MeshRefList **malloc_ref_list(MeshRefList **data, size_t length) {

  MeshRefList **alloc_list = malloc(length * sizeof(MeshRefList *));
  if (alloc_list == NULL) {
    VERBOSE_ERROR("Couldn't allocate raycast mesh ref list.\n");
    return NULL;
  }

  memcpy(alloc_list, data, length * sizeof(MeshRefList *));

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
  MeshRefList **alloc_list = malloc_ref_list(desc->mesh_lists, desc->length);
  if (alloc_list == NULL)
    return;

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
      .length = desc->length,
      .mesh_lists = alloc_list,
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
