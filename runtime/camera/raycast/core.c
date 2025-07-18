#include "core.h"
#include "hit_list.h"

/**
    ▗▄▄▖ ▗▄▖ ▗▄▄▖ ▗▄▄▄▖
   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌
   ▐▌   ▐▌ ▐▌▐▛▀▚▖▐▛▀▀▘
   ▝▚▄▄▖▝▚▄▞▘▐▌ ▐▌▐▙▄▄▖

 */

static inline MeshRefList **malloc_reflist(MeshRefList **, size_t);

/** allocate mesh_list on the heap
 TODO: When destroying the camera, and destroying the events, need to free
 this allocation as well
 Idea:
 For each html_add_event( { callback, destructor (optional) });
 */
MeshRefList **malloc_ref_list(MeshRefList **data, size_t length) {

  MeshRefList **alloc_list = malloc(length * sizeof(MeshRefList *));
  if (alloc_list == NULL) {
    perror("Couldn't allocate raycast mesh ref list.\n");
    return NULL;
  }

  memcpy(alloc_list, data, length * sizeof(MeshRefList *));

  return alloc_list;
}

/**
   Link to the camera a raycast system with the center of screen as raycast
   target. Useful for Flying or orbit mode in which cursor is usually hidden.
 */
void camera_raycast_center_hover(Camera *cam,
                                 const CameraRaycastDescriptor *desc) {

  // allocate mesh reference list for data lifetime sake
  MeshRefList **alloc_list = malloc_ref_list(desc->mesh_lists, desc->length);
  if (alloc_list == NULL)
    return;

  // allocate hit list on heap (sorted from closest hit to further)
  CameraRaycastHitList *hits_list = malloc(sizeof(CameraRaycastHitList));
  if (hits_list == NULL || camera_raycast_hit_list_create(
                               hits_list, CAMERA_RAYCAST_HIT_LIST_MAX_HIT) !=
                               CAMERA_RAYCAST_HIT_LIST_SUCCESS) {
    perror("Couldn't allocate camera raycast 'hit list'\n");
    return;
  }

  // convert data (add camera)
  const CameraRaycastCallbackData data = {
      // cb attributes
      .callback = desc->callback,
      .data = desc->data,

      // cast attributes
      .camera = cam,
      .viewport = desc->viewport,
      .hits = hits_list,

      // bound attributes
      .length = desc->length,
      .mesh_lists = alloc_list,
  };

  // define event callback
  em_mouse_callback_func event_callback = camera_raycast_event_callback_center;
  // add listener
  html_event_add_mouse_move(&(HTMLEventMouse){
      .callback = event_callback,
      .destructor = camera_raycast_event_destructor,
      .data = (void *)&data,
      .size = sizeof(CameraRaycastCallbackData),
      .owner = cam->id,
  });
}

void camera_raycast_center_click(Camera *cam,
                                 const CameraRaycastDescriptor *desc) {

  // allocate mesh reference list for data lifetime sake
  MeshRefList **alloc_list = malloc_ref_list(desc->mesh_lists, desc->length);
  if (alloc_list == NULL)
    return;

  // allocate hit list on heap (sorted from closest hit to further)
  CameraRaycastHitList *hits_list = malloc(sizeof(CameraRaycastHitList));
  if (hits_list == NULL || camera_raycast_hit_list_create(
                               hits_list, CAMERA_RAYCAST_HIT_LIST_MAX_HIT) !=
                               CAMERA_RAYCAST_HIT_LIST_SUCCESS) {
    perror("Couldn't allocate camera raycast 'hit list'\n");
    return;
  }

  // convert data (add camera)
  const CameraRaycastCallbackData data = {
      // cb attributes
      .callback = desc->callback,
      .data = desc->data,

      // cast attributes
      .camera = cam,
      .viewport = desc->viewport,
      .hits = hits_list,

      // bound attributes
      .length = desc->length,
      .mesh_lists = alloc_list,
  };

  // define event callback
  em_mouse_callback_func event_callback = camera_raycast_event_callback_center;
  // add listener
  html_event_add_mouse_down(&(HTMLEventMouse){
      .callback = event_callback, // html event callback
      .destructor = camera_raycast_event_destructor,
      .data = (void *)&data,
      .size = sizeof(CameraRaycastCallbackData),
      .owner = cam->id,
  });
}

/**
   Link to the camera a raycast system with the mouse position as raycast
   target. Useful for Edit mode.
 */
void camera_raycast_mouse_hover(Camera *cam,
                                const CameraRaycastDescriptor *desc) {

  // allocate mesh reference list for data lifetime sake
  MeshRefList **alloc_list = malloc_ref_list(desc->mesh_lists, desc->length);
  if (alloc_list == NULL)
    return;

  // allocate hit list on heap (sorted from closest hit to further)
  CameraRaycastHitList *hits_list = malloc(sizeof(CameraRaycastHitList));
  if (hits_list == NULL || camera_raycast_hit_list_create(
                               hits_list, CAMERA_RAYCAST_HIT_LIST_MAX_HIT) !=
                               CAMERA_RAYCAST_HIT_LIST_SUCCESS) {
    perror("Couldn't allocate camera raycast 'hit list'\n");
    return;
  }

  // convert data (add camera)
  const CameraRaycastCallbackData data = {
      // cb attributes
      .callback = desc->callback,
      .data = desc->data,

      // cast attributes
      .camera = cam,
      .viewport = desc->viewport,
      .hits = hits_list,

      // bound attributes
      .length = desc->length,
      .mesh_lists = alloc_list,
  };

  // define event callback
  em_mouse_callback_func event_callback = camera_raycast_event_callback_mouse;

  // add listener
  html_event_add_mouse_move(&(HTMLEventMouse){
      .callback = event_callback,
      .destructor = camera_raycast_event_destructor,
      .data = (void *)&data,
      .size = sizeof(CameraRaycastCallbackData),
      .owner = cam->id,
  });
}

void camera_raycast_mouse_click(Camera *cam,
                                const CameraRaycastDescriptor *desc) {

  // allocate mesh reference list for data lifetime sake
  MeshRefList **alloc_list = malloc_ref_list(desc->mesh_lists, desc->length);
  if (alloc_list == NULL)
    return;

  // allocate hit list on heap (sorted from closest hit to further)
  CameraRaycastHitList *hits_list = malloc(sizeof(CameraRaycastHitList));
  if (hits_list == NULL || camera_raycast_hit_list_create(
                               hits_list, CAMERA_RAYCAST_HIT_LIST_MAX_HIT) !=
                               CAMERA_RAYCAST_HIT_LIST_SUCCESS) {
    perror("Couldn't allocate camera raycast 'hit list'\n");
    return;
  }

  // convert data (add camera)
  const CameraRaycastCallbackData data = {
      // cb attributes
      .callback = desc->callback,
      .data = desc->data,

      // cast attributes
      .camera = cam,
      .viewport = desc->viewport,
      .hits = hits_list,

      // bound attributes
      .length = desc->length,
      .mesh_lists = alloc_list,
  };

  // define event callback
  em_mouse_callback_func event_callback = camera_raycast_event_callback_mouse;
  // add listener
  html_event_add_mouse_down(&(HTMLEventMouse){
      .callback = event_callback,
      .destructor = camera_raycast_event_destructor,
      .data = (void *)&data,
      .size = sizeof(CameraRaycastCallbackData),
      .owner = cam->id,
  });
}
