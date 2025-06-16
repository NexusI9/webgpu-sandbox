#include "uniform.h"
#include <string.h>

CameraUniform camera_uniform(Camera *c) {
  // Combine directly view matrix and camera position so faster to upload into
  // buffer

  CameraUniform cam_uni = (CameraUniform){0};

  // transform vec3 to vec4 for alignment sake
  vec4 pos_uniform = {c->position[0], c->position[1], c->position[2], 1.0f};
  vec4 lookat_uniform = {c->target[0], c->target[1], c->target[2], 1.0f};

  glm_vec4_copy(pos_uniform, cam_uni.position);
  glm_vec4_copy(lookat_uniform, cam_uni.lookat);
  glm_mat4_copy(c->view, cam_uni.view);
  cam_uni.mode = c->mode;

  return cam_uni;
}

void camera_uniform_update_matrix(void *callback_camera, void *data) {

  Camera *cast_cam = (Camera *)callback_camera;
  CameraUniform *new_data = (CameraUniform *)data;

  //  transfer updated camera values (position and view) to new data
  CameraUniform uCamera = camera_uniform(cast_cam);
  glm_mat4_copy(uCamera.view, new_data->view);
  glm_vec4_copy(uCamera.position, new_data->position);
  glm_vec4_copy(uCamera.lookat, new_data->lookat);
  new_data->mode = uCamera.mode;

}

/**
   Compare the callback camera new view with the last one to determine if the
   callback shall trigger
 */
bool camera_uniform_compare_views(void *callback_camera,
                                  const void *entry_data) {

  // cast void* to camera*
  Camera *cast_cam = (Camera *)callback_camera;
  CameraUniform *cast_uni = (CameraUniform *)entry_data;

  // compare two views
  return memcmp(cast_cam->view, cast_uni->view, sizeof(mat4)) != 0;
}
