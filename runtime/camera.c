#include "camera.h"

camera camera_create() {
  camera c;
  camera_reset(&c);
  return c;
}

void camera_reset(camera *c) {
  if (c) {
    glm_vec3_zero(c->position);
    glm_vec3_zero(c->euler_rotation);
    glm_mat4_identity(c->view);
  }
}

CameraUniform camera_uniform(camera *c) {
  // Combine directly view matrix and camera position so faster to upload into
  // buffer
  CameraUniform cam_uni;

  glm_vec3_copy(c->position, cam_uni.position);
  glm_mat4_copy(c->view, cam_uni.view);

  return cam_uni;
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

  float rot_z = camera->euler_rotation[2];

  mat4 new_view = (mat4){
      {cos(rot_z), 0.0f, sin(rot_z), 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {-sin(rot_z), 0.0, cos(rot_z), 0.0f},
      {-1 * camera->position[0], -1 * camera->position[1],
       -1 * camera->position[2], 1.0f},
  };

  glm_mat4_copy(new_view, camera->view);
}
