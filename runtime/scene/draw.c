#include "draw.h"
#include "../backend/renderer/renderer.h"
#include "../camera/camera.h"

/**
   Draw callback added to the Scene Renderer draw callbacks.
   Called before the scene renderer draw layouts.

   Basically udpate the camera matrix based on its mode and user input.
 */
void scene_camera_draw_callback(void *data) {

  Camera *cast_camera = (Camera *)data;

  // update camera
  camera_draw(cast_camera);
}
