#ifndef _CAMERA_RAYCAST_CORE_H_
#define _CAMERA_RAYCAST_CORE_H_

#include <string.h>

#include "./hit_list.h"
#include "emscripten/em_types.h"
#include "emscripten/html5.h"
#include "runtime/camera/core.h"
#include "runtime/html_event/html_event.h"
#include "runtime/input/input.h"
#include "runtime/mesh/mesh.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/raycast/core.h"
#include "runtime/raycast/raycast.h"
#include "runtime/viewport/core.h"
#include "runtime/viewport/viewport.h"

/**
   2 different types of raycast methods:
    1. To screen center
    2. To mouse position

   Camera raycast overall flow:

                          .----------------.
                          | camera raycast |
                          '----------------'
                .----------------'  '------------------.
          .----'----.                            .-----'---.
         |  Center  |                            |  Mouse  |
         '----.-----'                            '----.----'
      .-------'-------.                       .-------'-------.
   .--'---.       .---'---.                .--'---.       .---'---.
  | Hover |       | Click |               | Hover |       | Click |
  '---.---'       '---.---'               '---.---'       '---.---'
     ...   .----------'------------.         ...             ...
          |       Input Event      |
          |------------------------|
          | .--------------------. |
          | |   Event Callback   | |
          | |--------------------| |
          | |  .--------------.  | |
          | | |  Cast Method  |  | |
          | | '---------------'	 | |
          | '--------------------' |
          '------------------------'

  Since it would be very efficient to put the cast method in the draw loop for
  performace sake, the cast method is nested within the input event listener so
  it only gets updated on mouse movement.

  Note that since the upcoming scene will mostly be statics, this "mouse event"
  dependant approach can work. However is castable mesh are in movement, the
  current approach become inneficient as it won't detect moving meshes in the
  cast unless the mouse is moving.

 */

typedef enum {
  CameraRaycastBound_AABB,
  CameraRaycastBound_OBB,
} CameraRaycastBound;

typedef enum {
  CameraRaycastSpace_WorldSpace,
  CameraRaycastSpace_ScreenSpace,
} CameraRaycastSpace;

typedef enum {
  CameraRaycastTarget_ScreenCenter,
  CameraRaycastTarget_MousePosition,
} CameraRaycastTarget;

typedef enum {
  CameraRaycastEvent_MouseHover,
  CameraRaycastEvent_MouseDown,
} CameraRaycastEvent;

typedef enum {
  CameraRaycastAlloc_None = 0,
  CameraRaycastAlloc_IncludeList = 1 << 0,
  CameraRaycastAlloc_ExcludeList = 1 << 1,
  CameraRaycastAlloc_Data = 1 << 2,
  CameraRaycastAlloc_All = ~0,
} CameraRaycastAlloc;

typedef struct {
  const char *label;
  Raycast *raycast;
  CameraRaycastHitList *hits;
  // use last hit to prevent spamming update on hover
  // (only update if current hit != last hit)
  const CameraRaycastHit *last_hit;
} CameraRaycastCallback;

typedef void (*camera_raycast_callback)(CameraRaycastCallback *,
                                        const EmscriptenMouseEvent *, void *);

typedef void (*camera_raycast_destructor)(void *);

typedef struct {

  // raycast relative objects
  const char *label;
  Camera *camera;
  Viewport *viewport;
  CameraRaycastSpace space;
  float screen_space_size;
  CameraRaycastBound bound;

  // mesh lists to check
  MeshRefListArray *include;
  MeshRefListArray *exclude;

  // raycast result list
  CameraRaycastHitList *hits;
  CameraRaycastHit last_hit;

  // callback
  camera_raycast_callback callback;
  void *data;
  size_t size;
  // used for destructor to know which data shall be freed
  CameraRaycastAlloc alloc;

} CameraRaycastCallbackData;

typedef struct {

  // raycast
  const char *label;
  Viewport *viewport;
  CameraRaycastTarget target;
  CameraRaycastEvent event;
  CameraRaycastSpace space;
  float screen_space_size;
  CameraRaycastBound bound;

  // targets lists
  MeshRefListArray *include;
  MeshRefListArray *exclude;

  // callback
  camera_raycast_callback callback;
  void *data;
  size_t size;

} CameraRaycastDescriptor;

// raycast to screen center
void camera_raycast(Camera *, const CameraRaycastDescriptor *,
                    const CameraRaycastAlloc);

#endif
