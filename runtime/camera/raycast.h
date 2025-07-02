#ifndef _CAMERA_RAYCAST_H_
#define _CAMERA_RAYCAST_H_

#include "../mesh/mesh.h"
#include "../raycast/raycast.h"
#include "../viewport/viewport.h"

#include "core.h"

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

typedef struct {
  Raycast *raycast;
  Mesh *mesh;
} CameraRaycastCallback;

typedef void (*camera_raycast_callback)(CameraRaycastCallback*, void*);

typedef struct {
  const Camera *camera;
  const Viewport *viewport;
  MeshRefList **mesh_lists;
  size_t length;
  camera_raycast_callback callback;
  void *data;
} CameraRaycastCallbackData;

typedef struct {
  const char *target;
  MeshRefList **mesh_lists;
  size_t length;
  camera_raycast_callback callback;
  void *data;
  const Viewport *viewport;
} CameraRaycastDescriptor;

void camera_raycast_center_hover(const Camera *,
                                 const CameraRaycastDescriptor *);

void camera_raycast_center_click(const Camera *,
                                 const CameraRaycastDescriptor *);

void camera_raycast_mouse_hover(const Camera *,
                                const CameraRaycastDescriptor *);
void camera_raycast_mouse_click(const Camera *,
                                const CameraRaycastDescriptor *);

#endif
