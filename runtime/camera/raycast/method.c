#include "method.h"
#include "../../input/input.h"

/**
   ▗▄▄▖ ▗▄▖  ▗▄▄▖▗▄▄▄▖    ▗▖  ▗▖▗▄▄▄▖▗▄▄▄▖▗▖ ▗▖ ▗▄▖ ▗▄▄▄  ▗▄▄▖
  ▐▌   ▐▌ ▐▌▐▌     █      ▐▛▚▞▜▌▐▌     █  ▐▌ ▐▌▐▌ ▐▌▐▌  █▐▌
  ▐▌   ▐▛▀▜▌ ▝▀▚▖  █      ▐▌  ▐▌▐▛▀▀▘  █  ▐▛▀▜▌▐▌ ▐▌▐▌  █ ▝▀▚▖
  ▝▚▄▄▖▐▌ ▐▌▗▄▄▞▘  █      ▐▌  ▐▌▐▙▄▄▖  █  ▐▌ ▐▌▝▚▄▞▘▐▙▄▄▀▗▄▄▞▘

 */

/**
   Cast a ray towards the mouse position in screen space
 */
void camera_raycast_cast_method_mouse(Raycast *ray, Camera *cam, Viewport *vp) {

  // convert screen to NDC
  float x, y;
  input_mouse_NDC(g_input.mouse.x, g_input.mouse.y, vp->width, vp->height, &x,
                  &y);

  raycast_from_screen(ray, &cam->position, &cam->view, &vp->projection, x, y);
}

/**
   Cast a ray towards the center of the screen
 */
void camera_raycast_cast_method_center(Raycast *ray, Camera *cam,
                                       Viewport *vp) {

  // convert screen to NDC
  raycast_from_screen(ray, &cam->position, &cam->view, &vp->projection, 0.0f,
                      0.0f);
}
