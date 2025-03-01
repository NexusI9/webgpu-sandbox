#include "camera.h"
#include "../include/cglm/affine.h"
#include "../include/cglm/quat.h"
#include "../utils/math.h"
#include "../utils/system.h"
#include "constants.h"
#include "emscripten/html5.h"
#include "input.h"
#include "math.h"
#include "string.h"
#include <stdint.h>
#include <stdio.h>

camera camera_create(const CameraCreateDescriptor *cd) {
  camera c;

  // set matrix and position to 0
  camera_reset(&c);

  // assign additional attributes
  c.speed = cd->speed;
  c.clock = cd->clock;
  c.mode = cd->mode;

  camera_set_mode(&c, c.mode);
  return c;
}

void camera_reset(camera *c) {
  if (c) {
    glm_vec3_zero(c->position);
    glm_vec3_zero(c->euler_rotation);
    glm_mat4_identity(c->view);

    vec3 target = {0.0f, 0.0f, 0.0f};
    glm_vec3_copy(target, c->target);

    vec3 up = {0.0f, 1.0f, 0.0f};
    glm_vec3_copy(up, c->up);

    vec3 forward = {0.0f, 0.0f, 0.0f};
    glm_vec3_copy(forward, c->forward);

    vec3 right = {0.0f, 0.0f, 0.0f};
    glm_vec3_copy(right, c->right);
  }
}

static void camera_target_from_yaw_pitch(camera *camera, float yaw,
                                         float pitch) {

  /*
    update forward vector depending on yaw and pitch factor
    then update the camera target based on the position
    and the newly rotated forward
*/

  vec3 *forward = &camera->forward;
  vec3 *up = &camera->up;
  vec3 *right = &camera->right;

  // apply the yaw rotation along the up vector
  mat4 yaw_matrix;
  glm_rotate_make(yaw_matrix, yaw, *up);
  glm_mat4_mulv3(yaw_matrix, *forward, 1.0f, *forward);

  // apply pitch rotation along the right vector
  mat4 pitch_matrix;
  glm_rotate_make(pitch_matrix, pitch, *right);
  glm_mat4_mulv3(pitch_matrix, *forward, 1.0f, *forward);

  glm_vec3_add(camera->position, *forward, camera->target);
}

static void camera_flying_mode_controller(camera *camera) {

  // Define new position
  uint8_t boost = input_key(KEY_CAP) ? 3 : 1;

  float velocity = camera->speed * boost * camera->clock->delta;
  vec3 velo_vector = {velocity, velocity, velocity};

  vec3 velo_forward;
  glm_vec3_mul(velo_vector, camera->forward, velo_forward);

  vec3 velo_side;
  glm_vec3_mul(velo_vector, camera->right, velo_side);

  if (input_key(KEY_FORWARD_FR)) // Forward
    glm_vec3_add(camera->position, velo_forward, camera->position);

  if (input_key(KEY_BACKWARD_FR)) // Backward
    glm_vec3_sub(camera->position, velo_forward, camera->position);

  if (input_key(KEY_LEFT_FR)) // Left
    glm_vec3_sub(camera->position, velo_side, camera->position);

  if (input_key(KEY_RIGHT_FR)) // Right
    glm_vec3_add(camera->position, velo_side, camera->position);

  // Define new target from yaw and pitch
  // mouse movement > yaw pitch > forward vector > target vector
  float yaw = -g_input.mouse.movement.x * INPUT_MOUSE_SENSITIVITY *
              camera->clock->delta;
  float pitch = -g_input.mouse.movement.y * INPUT_MOUSE_SENSITIVITY *
                camera->clock->delta;

  camera_target_from_yaw_pitch(camera, yaw, pitch);

  // Update view matrix depending on new position and new target;
  camera_look_at(camera, camera->position, camera->target);
}

static float value = 0.0f;
static void camera_orbit_mode_controler(camera *camera) {

  float yaw = -g_input.mouse.x * INPUT_MOUSE_SENSITIVITY;
  float pitch = g_input.mouse.y * INPUT_MOUSE_SENSITIVITY;

  // TODO: dynamic radius based on mouse zoom or keyboard?
  float radius = glm_vec3_distance(camera->position, camera->target) +
                 g_input.mouse.wheel.deltaY * INPUT_WHEEL_SENSITIVITY;

  // 1. Create Picth & Yaw Quaternions

  // create quaternions
  versor q_pitch, q_yaw, q_final;

  // x axis rotation quaternions
  glm_quatv(q_pitch, pitch, (vec3){1.0f, 0.0f, 0.0f});

  // y axis rotation quaternions
  glm_quatv(q_yaw, yaw, (vec3){0.0f, 1.0f, 0.0f});

  // combine x & y quats
  glm_quat_mul(q_pitch, q_yaw, q_final);

  // 2. apply rotation to a BASE POSITION
  vec3 new_pos = {0.0f, 0.0f, radius}; // start at fixed distance

  // rotate base position using final quat
  glm_quat_rotatev(q_final, new_pos, new_pos);

  // 3. Offset position relative to target
  glm_vec3_add(new_pos, camera->target, camera->position);

  // glm_vec3_copy(new_pos, camera->position);
  camera_look_at(camera, camera->position, camera->target);
}

void camera_draw(camera *camera) {

  switch (camera->mode) {

  case FLYING:
    camera_flying_mode_controller(camera);
    return;

  case ORBIT:
    camera_orbit_mode_controler(camera);
    return;

  case FIXED:
    // remove event listeners
  default:
    return;
  }

  camera_update_view(camera);
}

void camera_set_mode(camera *camera, CameraMode mode) { camera->mode = mode; }

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

void camera_update_matrix_uniform(void *callback_camera, void *data) {

  camera *cast_cam = (camera *)callback_camera;
  CameraUniform *new_data = (CameraUniform *)data;

  // transfer updated camera values (position and view)
  CameraUniform uCamera = camera_uniform(cast_cam);
  glm_mat4_copy(uCamera.view, new_data->view);
  glm_vec4_copy(uCamera.position, new_data->position);
}

void camera_translate(camera *camera, vec3 new_position) {
  // get the absolute value, need to transfom the new position into
  // the camera coordinate system (relative)
  // https://www.ogldev.org/www/tutorial13/tutorial13.html

  camera->position[0] += new_position[0];
  camera->position[1] += new_position[1];
  camera->position[2] += new_position[2];

  // camera_update_view(camera);
}

void camera_rotate(camera *camera, vec3 new_rotation) {
  glm_vec3_copy(new_rotation, camera->euler_rotation);
  // camera_update_view(camera);
}

void camera_update_view(camera *camera) {
  // Yaw-pitch-roll camera (1st approach)
  // Depends on camera_rotate/translate => update_view

  float rot_x = camera->euler_rotation[0];
  float rot_y = camera->euler_rotation[1];
  float rot_z = camera->euler_rotation[2];

  mat4 new_view = (mat4){
      {
          cos(rot_y) * cos(rot_z),
          cos(rot_y) * sin(rot_z),
          -sin(rot_y),
          0.0f,
      },

      {
          sin(rot_x) * sin(rot_y) * cos(rot_z) - cos(rot_x) * sin(rot_z),
          sin(rot_x) * sin(rot_y) * sin(rot_z) + cos(rot_x) * cos(rot_z),
          sin(rot_x) * cos(rot_y),
          0.0f,
      },

      {
          cos(rot_x) * sin(rot_y) * cos(rot_z) + sin(rot_x) * sin(rot_z),
          cos(rot_x) * sin(rot_y) * sin(rot_z) - sin(rot_x) * cos(rot_z),
          cos(rot_x) * cos(rot_y),
          0.0f,
      },

      {
          -1 * camera->position[0],
          -1 * camera->position[1],
          -1 * camera->position[2],
          1.0f,
      },
  };

  glm_mat4_copy(new_view, camera->view);
}

void camera_look_at(camera *camera, vec3 position, vec3 target) {
  // UVN camera (2nd approach)

  vec3 *forward = &camera->forward;
  vec3 *up = &camera->up;
  vec3 *right = &camera->right;

  glm_vec3_sub(target, position, *forward);
  glm_vec3_normalize(*forward);

  vec3 world_up = {0.0f, 1.0f, 0.0f};

  // need to avoid forward being parallel to world_up
  if (fabs(glm_vec3_dot(*forward, world_up)) > 0.99f) {
    // use x axis as up instead
    vec3 correct_up = {0.0f, 0.0f, 1.0f};
    glm_vec3_copy(correct_up, world_up);
  }

  // set right vector
  glm_vec3_cross(*forward, world_up, *right);
  glm_vec3_normalize(*right);

  // set  up vector
  glm_vec3_cross(*right, *forward, *up);

  // create new view matrix
  mat4 view = {
      {
          (*right)[0],
          (*up)[0],
          (*forward)[0],
          0.0f,
      },
      {
          (*right)[1],
          (*up)[1],
          (*forward)[1],
          0.0f,
      },
      {
          (*right)[2],
          (*up)[2],
          (*forward)[2],
          0.0f,
      },
      {
          -glm_vec3_dot(*right, position),
          -glm_vec3_dot(*up, position),
          -glm_vec3_dot(*forward, position),
          1.0f,
      },
  };

  // update camera view
  // TODO : look like glm_..._copy is flawed
  // Can be related to alignment issue
  // Try with : alignas(16) mat4 view;
  memcpy(camera->position, position, sizeof(vec3));
  memcpy(camera->view, view, sizeof(mat4));
}
