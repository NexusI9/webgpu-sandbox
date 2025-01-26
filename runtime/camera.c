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
