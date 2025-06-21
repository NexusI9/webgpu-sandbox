#include "mode.h"
#include "../input/input.h"
#include "core.h"

static void camera_target_from_yaw_pitch(Camera *, float, float);

/**
    Update forward vector depending on yaw and pitch factor
    then update the camera target based on the position
    and the newly rotated forward
*/
void camera_target_from_yaw_pitch(Camera *camera, float yaw, float pitch) {

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

/**
   Set camera flying mode on global Input pooling
 */
void camera_mode_flying_controller(Camera *camera) {

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
  camera_lookat(camera, camera->position, camera->target);
}

/**
   Set camera orbit mode based on global Input pooling
 */
void camera_mode_orbit_controller(Camera *camera) {

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
  camera_lookat(camera, camera->position, camera->target);
}

/**
   Define camera edit mode
 */
void camera_mode_edit_controller(Camera *camera) {

  vec3 up = {0.0f, 1.0f, 0.0f};

  // CMD + WHEEL = Zoom
  if (input_key(INPUT_KEY_CMD)) {
    // TODO: dynamic radius based on mouse zoom or keyboard?
    float radius = glm_vec3_distance(camera->position, camera->target) +
                   g_input.mouse.wheel.deltaY * camera->wheel_sensitivity;

    if (radius < 0.1f)
      radius = 1.0f;

    vec3 dir;
    glm_vec3_sub(camera->position, camera->target, dir);
    glm_vec3_normalize(dir);
    glm_vec3_scale(dir, radius, dir);
    glm_vec3_add(camera->target, dir, camera->position);
  }
  // CAP+ WHEEL = Move
  else if (input_key(INPUT_KEY_CAP)) {

    float x = g_input.mouse.wheel.deltaX * camera->wheel_sensitivity;
    float y = g_input.mouse.wheel.deltaY * camera->wheel_sensitivity;

#ifdef CAMERA_MODE_EDIT_INVERT_X
    x *= -1;
#endif

#ifdef CAMERA_MODE_EDIT_INVERT_Y
    y *= -1;
#endif

    vec3 fwd;
    glm_vec3_sub(camera->target, camera->position, fwd);
    glm_vec3_normalize(fwd);

    vec3 right;
    glm_vec3_crossn(fwd, up, right);

    vec3 move = {0};
    glm_vec3_scale(right, x, move);

    vec3 up_move = {0};
    glm_vec3_scale(up, y, up_move);
    glm_vec3_add(move, up_move, move);

    glm_vec3_add(camera->position, move, camera->position);
    glm_vec3_add(camera->target, move, camera->target);
  }
  // WHEEL = Orbit
  else {

    float yaw = -g_input.mouse.wheel.deltaX * camera->wheel_sensitivity;
    float pitch = g_input.mouse.wheel.deltaY * camera->wheel_sensitivity;

    vec3 dir;
    glm_vec3_sub(camera->position, camera->target, dir);

    vec3 right;
    glm_vec3_crossn(dir, up, right);

    // 1. Create Picth & Yaw Quaternions

    // create quaternions
    versor q_pitch, q_yaw;

    // Pitch: x axis rotation quaternions
    glm_quatv(q_pitch, pitch, right);
    glm_quat_rotatev(q_pitch, dir, dir);

    // Yaw: y axis rotation quaternions
    glm_quatv(q_yaw, yaw, up);
    glm_quat_rotatev(q_yaw, dir, dir);

    // 3. Offset position relative to target
    glm_vec3_add(camera->target, dir, camera->position);
  }

  camera_lookat(camera, camera->position, camera->target);
}

void camera_set_mode(Camera *camera, CameraMode mode) { camera->mode = mode; }
