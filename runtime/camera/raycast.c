#include "camera.h"



/**
   Link to the camera a raycast system with the center of screen as raycast
   target. Useful for Flying or orbit mode in which cursor is usually hidden.
 */
void camera_raycast_center(Camera *cam, const CameraRaycastDescriptor * desc) {}

/**
   Link to the camera a raycast system with the mouse position as raycast
   target. Useful for Edit mode.
 */
void camera_raycast_mouse(Camera *cam, const CameraRaycastDescriptor * desc) {
  
}
