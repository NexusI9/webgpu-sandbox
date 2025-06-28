#include "raycast.h"
#include "../input/input.h"
#include "../raycast/raycast.h"
#include "emscripten/html5.h"

/**
   ▗▄▄▖ ▗▄▖  ▗▄▄▖▗▄▄▄▖    ▗▖  ▗▖▗▄▄▄▖▗▄▄▄▖▗▖ ▗▖ ▗▄▖ ▗▄▄▄  ▗▄▄▖
  ▐▌   ▐▌ ▐▌▐▌     █      ▐▛▚▞▜▌▐▌     █  ▐▌ ▐▌▐▌ ▐▌▐▌  █▐▌
  ▐▌   ▐▛▀▜▌ ▝▀▚▖  █      ▐▌  ▐▌▐▛▀▀▘  █  ▐▛▀▜▌▐▌ ▐▌▐▌  █ ▝▀▚▖
  ▝▚▄▄▖▐▌ ▐▌▗▄▄▞▘  █      ▐▌  ▐▌▐▙▄▄▖  █  ▐▌ ▐▌▝▚▄▞▘▐▙▄▄▀▗▄▄▞▘

 */
typedef void (*camera_raycast_cast_method)(Raycast *, Camera *, Viewport *);
static void camera_raycast_cast_method_center(Raycast *ray, Camera *cam,
                                              Viewport *vp);
static void camera_raycast_cast_method_mouse(Raycast *ray, Camera *cam,
                                             Viewport *vp);
static void camera_raycast_cast_method_screen(Raycast *, Camera *, Viewport *,
                                              float, float);

typedef struct {

  // raycast attribute and cast method (from center or mouse position)
  Camera *camera;
  Viewport *viewport;
  camera_raycast_cast_method cast_method;

  // mesh list raycast is tested against
  MeshRefList **mesh_lists;
  size_t length;

  // on move attribtues
  camera_raycast_callback callback;
  void *data;
  size_t size;
} CameraRaycastCheckBoundsDescriptor;

static void
camera_raycast_check_bounds(const CameraRaycastCheckBoundsDescriptor *);

/**
   Cast ray at a given
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

/**
   Traverse the meshes ref lists and check if the
 */
void camera_raycast_check_bounds(
    const CameraRaycastCheckBoundsDescriptor *desc) {

  printf("raycast\n");
  /*Raycast raycast;
  // go though each meshes of each ref lists and check bound
  for (size_t l = 0; l < desc->length; l++) {

    MeshRefList *ref_list = desc->mesh_lists[l];

    for (size_t m = 0; m < ref_list->length; m++) {
      Mesh *mesh = ref_list->entries[m];

      // check if raycast within mesh bound
      desc->cast_method(&raycast, desc->camera, desc->viewport);
    }
  }*/
};

/**
   ▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖     ▗▄▄▖ ▗▄▖ ▗▖   ▗▖   ▗▄▄▖  ▗▄▖  ▗▄▄▖▗▖ ▗▖ ▗▄▄▖
   ▐▌   ▐▌  ▐▌▐▌   ▐▛▚▖▐▌  █      ▐▌   ▐▌ ▐▌▐▌   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌▗▞▘▐▌
   ▐▛▀▀▘▐▌  ▐▌▐▛▀▀▘▐▌ ▝▜▌  █      ▐▌   ▐▛▀▜▌▐▌   ▐▌   ▐▛▀▚▖▐▛▀▜▌▐▌   ▐▛▚▖  ▝▀▚▖
   ▐▙▄▄▖ ▝▚▞▘ ▐▙▄▄▖▐▌  ▐▌  █      ▝▚▄▄▖▐▌ ▐▌▐▙▄▄▖▐▙▄▄▖▐▙▄▞▘▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌▗▄▄▞▘

 */

static bool
camera_raycast_event_callback_center(int, const EmscriptenMouseEvent *, void *);
static bool
camera_raycast_event_callback_mouse(int, const EmscriptenMouseEvent *, void *);

bool camera_raycast_event_callback_center(
    int eventType, const EmscriptenMouseEvent *mouseEvent, void *data) {

  // convert data
  CameraRaycastCallbackData *cast_data = (CameraRaycastCallbackData *)data;

  // select cast method
  camera_raycast_cast_method method = camera_raycast_cast_method_center;

  // call common checker
  camera_raycast_check_bounds(&(CameraRaycastCheckBoundsDescriptor){
      .camera = cast_data->camera,
      .viewport = cast_data->viewport,
      .cast_method = method,
      .callback = cast_data->callback,
      .length = cast_data->length,
      .data = cast_data->data,
      .mesh_lists = cast_data->mesh_lists,
      .size = cast_data->size,
  });

  return false;
}

bool camera_raycast_event_callback_mouse(int eventType,
                                         const EmscriptenMouseEvent *mouseEvent,
                                         void *data) {

  // convert data
  CameraRaycastCallbackData *cast_data = (CameraRaycastCallbackData *)data;

  // select cast method
  camera_raycast_cast_method method = camera_raycast_cast_method_mouse;

  // call common checker
  camera_raycast_check_bounds(&(CameraRaycastCheckBoundsDescriptor){
      .camera = cast_data->camera,
      .viewport = cast_data->viewport,
      .cast_method = method,
      .callback = cast_data->callback,
      .length = cast_data->length,
      .data = cast_data->data,
      .mesh_lists = cast_data->mesh_lists,
      .size = cast_data->size,
  });

  return false;
}

/**
    ▗▄▄▖ ▗▄▖ ▗▄▄▖ ▗▄▄▄▖
   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌
   ▐▌   ▐▌ ▐▌▐▛▀▚▖▐▛▀▀▘
   ▝▚▄▄▖▝▚▄▞▘▐▌ ▐▌▐▙▄▄▖

 */

/**
   Link to the camera a raycast system with the center of screen as raycast
   target. Useful for Flying or orbit mode in which cursor is usually hidden.
 */
void camera_raycast_center_hover(const Camera *cam,
                                 const CameraRaycastDescriptor *desc) {
  // define event callback
  em_mouse_callback_func event_callback = camera_raycast_event_callback_center;
  // add listener
  input_on_mouse_move(desc->target, event_callback, desc->data);
}

void camera_raycast_center_click(const Camera *cam,
                                 const CameraRaycastDescriptor *desc) {

  // define event callback
  em_mouse_callback_func event_callback = camera_raycast_event_callback_center;
  // add listener
  input_on_mouse_down(desc->target, event_callback, desc->data);
}

/**
   Link to the camera a raycast system with the mouse position as raycast
   target. Useful for Edit mode.
 */
void camera_raycast_mouse_hover(const Camera *cam,
                                const CameraRaycastDescriptor *desc) {

  // define event callback
  em_mouse_callback_func event_callback = camera_raycast_event_callback_mouse;
  // add listener
  input_on_mouse_move(desc->target, event_callback, desc->data);
}

void camera_raycast_mouse_click(const Camera *cam,
                                const CameraRaycastDescriptor *desc) {

  // define event callback
  em_mouse_callback_func event_callback = camera_raycast_event_callback_mouse;
  // add listener
  input_on_mouse_down(desc->target, event_callback, desc->data);
}
