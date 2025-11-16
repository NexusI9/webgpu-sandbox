#include "core.h"

#include <emscripten/html5.h>
#include <stdlib.h>
#include <string.h>

#include "backend/logger.h"
#include "callback.h"
#include "hit_list.h"
#include "runtime/camera/core.h"
#include "runtime/html_event/add.h"
#include "runtime/html_event/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"

/**
    ▗▄▄▖ ▗▄▖ ▗▄▄▖ ▗▄▄▄▖
   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌
   ▐▌   ▐▌ ▐▌▐▛▀▚▖▐▛▀▀▘
   ▝▚▄▄▖▝▚▄▞▘▐▌ ▐▌▐▙▄▄▖

 */

static inline void camera_raycast_create_event(Camera *,
                                               const CameraRaycastDescriptor *,
                                               em_mouse_callback_func,
                                               html_event_mouse,
                                               const CameraRaycastAlloc);

/**
   Since the html event and the loop are not part of the same "timeline"
   We need to allocate the event data on the heap so it doesn't go out of scope.
   However we passed a destructor in the html_event listener to ensure the data
   won't stagnate once we remove the owner objects.
 */
void camera_raycast_create_event(Camera *cam,
                                 const CameraRaycastDescriptor *desc,
                                 em_mouse_callback_func em_callback,
                                 html_event_mouse html_event_callback,
                                 const CameraRaycastAlloc alloc) {

  // === ALLOCATE MESH REFERENCES ===
  MeshRefListArray *include = desc->include, *exclude = desc->exclude;

  if ((CameraRaycastAlloc_IncludeList & alloc) && desc->include) {
    include = malloc(sizeof(MeshRefListArray));
    mesh_ref_list_array_copy(desc->include, include);
  }

  if ((CameraRaycastAlloc_ExcludeList & alloc) && desc->exclude) {
    exclude = malloc(sizeof(MeshRefListArray));
    mesh_ref_list_array_copy(desc->exclude, exclude);
  }

  // === ALLOCATE HIT LIST ===
  // (sorted from closest hit to further)
  CameraRaycastHitList *hits_list = malloc(sizeof(CameraRaycastHitList));
  if (hits_list == NULL || camera_raycast_hit_list_create(
                               hits_list, CAMERA_RAYCAST_HIT_LIST_MAX_HIT) !=
                               CameraRaycastHitListStatus_Success) {
    logger_add(LoggerFlag_Error,
               "Couldn't allocate camera raycast 'hit list'\n");
    return;
  }

  // === USER DATA ===
  void *alloc_data = desc->data;

  if ((CameraRaycastAlloc_Data & alloc) && desc->size > 0) {
    alloc_data = malloc(desc->size);
    if (alloc_data == NULL) {
      logger_add(LoggerFlag_Error, "Couldn't allocate camera raycast 'data'\n");
      return;
    }
    memcpy(alloc_data, desc->data, desc->size);
  }

  // convert data (add camera and hit list)
  const CameraRaycastCallbackData data = {
      // cb attributes
      .label = desc->label,
      .callback = desc->callback,
      .data = alloc_data,
      .size = desc->size, // DELETEME ?
      .alloc = alloc,

      // cast attributes
      .camera = cam,
      .viewport = desc->viewport,
      .hits = hits_list,
      .screen_space_size = desc->screen_space_size,
      .space = desc->space,
      .bound = desc->bound,

      // bound attributes
      .include = include,
      .exclude = exclude,
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

   Dispatch the different callbacks based on raycast configuration: mouse vs
   screen center, world vs screen space

   ▗▖  ▗▖ ▗▄▖ ▗▖ ▗▖ ▗▄▄▖▗▄▄▄▖
   ▐▛▚▞▜▌▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌
   ▐▌  ▐▌▐▌ ▐▌▐▌ ▐▌ ▝▀▚▖▐▛▀▀▘
   ▐▌  ▐▌▝▚▄▞▘▝▚▄▞▘▗▄▄▞▘▐▙▄▄▖

   Link to the camera a raycast system with the mouse position as raycast
   target. Useful for Edit mode.

    ▗▄▄▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▄▄▄▖▗▄▄▖
   ▐▌   ▐▌   ▐▛▚▖▐▌  █  ▐▌   ▐▌ ▐▌
   ▐▌   ▐▛▀▀▘▐▌ ▝▜▌  █  ▐▛▀▀▘▐▛▀▚▖
   ▝▚▄▄▖▐▙▄▄▖▐▌  ▐▌  █  ▐▙▄▄▖▐▌ ▐▌

   Link to the camera a raycast system with the center of screen as raycast
   target. Useful for Flying or orbit mode in which cursor is usually hidden.
 */

// define event type
static const html_event_mouse html_event_callbacks[] = {
    [CameraRaycastEvent_MouseDown] = html_event_add_mouse_down,
    [CameraRaycastEvent_MouseHover] = html_event_add_mouse_move,
};

// define target callback (mouse position | screen center)
static const em_mouse_callback_func em_mouse_callbacks[] = {
    [CameraRaycastTarget_MousePosition] = camera_raycast_event_callback_mouse,
    [CameraRaycastTarget_ScreenCenter] = camera_raycast_event_callback_center,
};

void camera_raycast(Camera *cam, const CameraRaycastDescriptor *desc,
                    const CameraRaycastAlloc alloc) {
  html_event_mouse html_event_callback = html_event_callbacks[desc->event];
  em_mouse_callback_func em_callback = em_mouse_callbacks[desc->target];
  camera_raycast_create_event(cam, desc, camera_raycast_event_callback_mouse,
                              html_event_callback, alloc);
}
