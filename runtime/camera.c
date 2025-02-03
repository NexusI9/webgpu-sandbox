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
    glm_mat4_identity(c->view_matrix);
  }
}

CameraUniform camera_uniform(camera *c) {
  // Combine directly view matrix and camera position so faster to upload into
  // buffer
  CameraUniform cam_uni;

  glm_vec3_copy(c->position, cam_uni.position);
  glm_mat4_copy(c->view_matrix, cam_uni.view);

  return cam_uni;
}
