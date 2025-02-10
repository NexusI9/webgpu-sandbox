#include "camera.h"
#include "constants.h"
#include "emscripten/html5.h"
#include <stdio.h>

camera camera_create() {
  camera c;
  camera_reset(&c);
  c.speed = 0.1f;
  return c;
}

void camera_reset(camera *c) {
  if (c) {
    glm_vec3_zero(c->position);
    glm_vec3_zero(c->euler_rotation);
    glm_mat4_identity(c->view);
  }
}

static bool camera_flying_keyboard_callback(
    int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData) {

  camera *cam = (camera *)userData;

  printf("key code:%d\n", keyEvent->keyCode);
  switch (keyEvent->keyCode) {

  case KEY_FORWARD_FR: {
    camera_translate(cam, (vec3){
                              cam->position[0],
                              cam->position[1],
                              cam->position[2],
                          });
    break;
  }

  case KEY_BACKWARD_FR:
    break;

  case KEY_LEFT_FR:
    break;

  case KEY_RIGHT_FR:
    break;

  default:
    break;
  }

  camera_update_view(cam);

  return false;
}

void camera_set_mode(camera *camera, CameraMode mode) {

  const char *target = EVENT_DEFAULT_TARGET;

  // set event listeners depending on camera mode
  switch (mode) {

  case FLYING:
    emscripten_set_keydown_callback(target, camera, false,
                                    camera_flying_keyboard_callback);
    return;

  case ORBIT:

    return;

  case FIXED:
  default:
    return;
  }
}

CameraUniform camera_uniform(camera *c) {
  // Combine directly view matrix and camera position so faster to upload into
  // buffer

  CameraUniform cam_uni;

  // transform vec3 to vec4 for alignment sake
  vec4 pos_uniform =
      (vec4){c->position[0], c->position[1], c->position[2], 1.0f};

  glm_vec4_copy(pos_uniform, cam_uni.position);
  glm_mat4_copy(c->view, cam_uni.view);

  return cam_uni;
}

void camera_update_uniform(void *callback_camera, void *data) {

  camera *cast_cam = (camera *)callback_camera;
  CameraUniform *new_data = (CameraUniform *)data;

  // transfer updated camera values
  CameraUniform uCamera = camera_uniform(cast_cam);
  glm_mat4_copy(uCamera.view, new_data->view);
  glm_vec4_copy(uCamera.position, new_data->position);
}

void camera_translate(camera *camera, vec3 new_position) {
  glm_vec3_copy(new_position, camera->position);
  camera_update_view(camera);
}

void camera_rotate(camera *camera, vec3 new_rotation) {
  glm_vec3_copy(new_rotation, camera->euler_rotation);
  camera_update_view(camera);
}

void camera_update_view(camera *camera) {

  float rot_x = camera->euler_rotation[0];
  float rot_y = camera->euler_rotation[1];
  float rot_z = camera->euler_rotation[2];

  mat4 new_view = (mat4){
      {cos(rot_y) * cos(rot_z), cos(rot_y) * sin(rot_z), -sin(rot_y), 0.0f},

      {sin(rot_x) * sin(rot_y) * cos(rot_z) - cos(rot_x) * sin(rot_z),
       sin(rot_x) * sin(rot_y) * sin(rot_z) + cos(rot_x) * cos(rot_z),
       sin(rot_x) * cos(rot_y), 0.0f},

      {cos(rot_x) * sin(rot_y) * cos(rot_z) + sin(rot_x) * sin(rot_z),
       cos(rot_x) * sin(rot_y) * sin(rot_z) - sin(rot_x) * cos(rot_z),
       cos(rot_x) * cos(rot_y), 0.0f},

      {-1 * camera->position[0], -1 * camera->position[1],
       -1 * camera->position[2], 1.0f},
  };

  glm_mat4_copy(new_view, camera->view);
}
