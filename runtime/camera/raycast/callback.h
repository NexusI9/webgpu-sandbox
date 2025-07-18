#ifndef _CAMERA_RAYCAST_CALLBACK_H_
#define _CAMERA_RAYCAST_CALLBACK_H_

#include "../core.h"
#include "../../viewport/viewport.h"
#include "../../raycast/raycast.h"
#include "./hit_list.h"

typedef struct {
  Raycast *raycast;
  CameraRaycastHitList *hits;
} CameraRaycastCallback;

typedef void (*camera_raycast_callback)(CameraRaycastCallback *,
                                        const EmscriptenMouseEvent *, void *);

typedef void (*camera_raycast_destructor)(void *);

typedef struct {

  // raycast relative objects
  Camera *camera;
  Viewport *viewport;

  // mesh lists to check
  MeshRefList **mesh_lists;
  size_t length;

  // raycast result list
  CameraRaycastHitList *hits;

  // callback
  camera_raycast_callback callback;
  void *data;

} CameraRaycastCallbackData;


bool
camera_raycast_event_callback_center(int, const EmscriptenMouseEvent *, void *);
bool
camera_raycast_event_callback_mouse(int, const EmscriptenMouseEvent *, void *);

bool camera_raycast_event_destructor(void *);
#endif
