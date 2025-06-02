#include "core.h"
#include "../../utils/math.h"
#include "../../utils/system.h"
#include "emscripten/html5.h"
#include "../input/input.h"
#include "math.h"
#include "string.h"
#include <cglm/cglm.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static int camera_list_expand(CameraList *);

void camera_create(Camera *cam, const CameraCreateDescriptor *cd) {

  // set matrix and position to 0
  camera_reset(cam);

  // assign additional attributes
  cam->speed = cd->speed;
  cam->clock = cd->clock;
  cam->mode = cd->mode;
  cam->sensitivity = cd->sensitivity;
  cam->wheel_sensitivity = cd->wheel_sensitivity;
}

void camera_reset(Camera *c) {
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

static void camera_target_from_yaw_pitch(Camera *camera, float yaw,
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

static void camera_flying_mode_controller(Camera *camera) {

  // Define new position
  uint8_t boost = input_key(INPUT_KEY_CAP) ? 3 : 1;

  float velocity = camera->speed * boost * camera->clock->delta;
  vec3 velo_vector = {velocity, velocity, velocity};

  vec3 velo_forward;
  glm_vec3_mul(velo_vector, camera->forward, velo_forward);

  vec3 velo_side;
  glm_vec3_mul(velo_vector, camera->right, velo_side);

  if (input_key(INPUT_KEY_FORWARD_FR)) // Forward
    glm_vec3_add(camera->position, velo_forward, camera->position);

  if (input_key(INPUT_KEY_BACKWARD_FR)) // Backward
    glm_vec3_sub(camera->position, velo_forward, camera->position);

  if (input_key(INPUT_KEY_LEFT_FR)) // Left
    glm_vec3_add(camera->position, velo_side, camera->position);

  if (input_key(INPUT_KEY_RIGHT_FR)) // Right
    glm_vec3_sub(camera->position, velo_side, camera->position);

  // Define new target from yaw and pitch
  // mouse movement > yaw pitch > forward vector > target vector
  float yaw =
      -g_input.mouse.movement.x * camera->sensitivity * camera->clock->delta;
  float pitch =
      g_input.mouse.movement.y * camera->sensitivity * camera->clock->delta;

  camera_target_from_yaw_pitch(camera, yaw, pitch);

  // Update view matrix depending on new position and new target;
  camera_look_at(camera, camera->position, camera->target);
}

static void camera_orbit_mode_controler(Camera *camera) {

  float yaw = -g_input.mouse.x * camera->sensitivity;
  float pitch = g_input.mouse.y * camera->sensitivity;

  // TODO: dynamic radius based on mouse zoom or keyboard?
  float radius = glm_vec3_distance(camera->position, camera->target) +
                 g_input.mouse.wheel.deltaY * camera->wheel_sensitivity;

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

void camera_draw(Camera *camera) {

  switch (camera->mode) {

  case CameraMode_Flying:
    camera_flying_mode_controller(camera);
    return;

  case CameraMode_Orbit:
    camera_orbit_mode_controler(camera);
    return;

  case CameraMode_Fixed:
    // remove event listeners
  default:
    return;
  }

  camera_update_view(camera);
}

void camera_set_mode(Camera *camera, CameraMode mode) { camera->mode = mode; }

void camera_translate(Camera *camera, vec3 new_position) {
  // get the absolute value, need to transfom the new position into
  // the camera coordinate system (relative)
  // https://www.ogldev.org/www/tutorial13/tutorial13.html

  camera->position[0] += new_position[0];
  camera->position[1] += new_position[1];
  camera->position[2] += new_position[2];

  // camera_update_view(camera);
}

void camera_rotate(Camera *camera, vec3 new_rotation) {
  glm_vec3_copy(new_rotation, camera->euler_rotation);
  // camera_update_view(camera);
}

void camera_update_view(Camera *camera) {
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

void camera_look_at(Camera *camera, vec3 position, vec3 target) {

  // update camera view
  // TODO : look like glm_..._copy is flawed
  // Can be related to alignment issue
  // Try with : alignas(16) mat4 view;
  memcpy(camera->position, position, sizeof(vec3));

  vec3 *forward = &camera->forward;
  vec3 *up = &camera->up;
  vec3 *right = &camera->right;

  vec3 adjusted_up = (vec3){0.0f, 1.0f, 0.0f};

  glm_vec3_sub(target, position, *forward);
  glm_normalize(*forward);

  // need to avoid forward being parallel to world_up
  // use x axis as up instead
  if (fabs(glm_vec3_dot(*forward, *up)) > 0.99f)
    glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, adjusted_up);

  // calculate right vector
  glm_vec3_cross(*up, *forward, *right);
  glm_normalize(*right);

  glm_lookat(camera->position, target, adjusted_up, camera->view);
}

mat4 *camera_view(Camera *camera) { return &camera->view; }
