#include "method.h"
#include "../../input/input.h"

static void camera_raycast_cast_method_screen(Raycast *, Camera *, Viewport *,
					      float, float);



/**
   ▗▄▄▖ ▗▄▖  ▗▄▄▖▗▄▄▄▖    ▗▖  ▗▖▗▄▄▄▖▗▄▄▄▖▗▖ ▗▖ ▗▄▖ ▗▄▄▄  ▗▄▄▖
  ▐▌   ▐▌ ▐▌▐▌     █      ▐▛▚▞▜▌▐▌     █  ▐▌ ▐▌▐▌ ▐▌▐▌  █▐▌
  ▐▌   ▐▛▀▜▌ ▝▀▚▖  █      ▐▌  ▐▌▐▛▀▀▘  █  ▐▛▀▜▌▐▌ ▐▌▐▌  █ ▝▀▚▖
  ▝▚▄▄▖▐▌ ▐▌▗▄▄▞▘  █      ▐▌  ▐▌▐▙▄▄▖  █  ▐▌ ▐▌▝▚▄▞▘▐▙▄▄▀▗▄▄▞▘

 */



/**
   Cast ray at a given mouse position
 */
void camera_raycast_cast_method_screen(Raycast *ray, Camera *cam, Viewport *vp,
                                       float x, float y) {

  // near plane point in clip space
  vec4 ray_clip = {x, y, -1.0f, 1.0f};

  // unproject to world space
  mat4 inv_proj, inv_view;

  glm_mat4_inv(vp->projection, inv_proj);
  glm_mat4_inv(cam->view, inv_view);

  // eye space (remove projection)
  vec4 ray_eye;
  glm_mat4_mulv(inv_proj, ray_clip, ray_eye);
  // direction in eye space
  ray_eye[2] = -1.0f;
  ray_eye[3] = 0.0f;

  // world space (remove view)
  vec4 ray_world;
  glm_mat4_mulv(inv_view, ray_eye, ray_world);
  vec3 ray_dir = {ray_world[0], ray_world[1], ray_world[2]};
  glm_vec3_normalize(ray_dir);

  // set origin
  glm_vec3_zero(ray->origin);
  glm_vec3_copy(cam->position, ray->origin);

  // set direction
  glm_vec3_zero(ray->direction);
  glm_vec3_copy(ray_dir, ray->direction);
}

/**
   Cast a ray towards the mouse position in screen space
 */
void camera_raycast_cast_method_mouse(Raycast *ray, Camera *cam, Viewport *vp) {

  // convert screen to NDC
  float x = (2.0f * g_input.mouse.x) / vp->width - 1.0f;
  float y = 1.0f - (2.0f * g_input.mouse.y) / vp->height;

  camera_raycast_cast_method_screen(ray, cam, vp, x, y);
}

/**
   Cast a ray towards the center of the screen
 */
void camera_raycast_cast_method_center(Raycast *ray, Camera *cam,
                                       Viewport *vp) {

  // convert screen to NDC
  camera_raycast_cast_method_screen(ray, cam, vp, 0.0f, 0.0f);
}
