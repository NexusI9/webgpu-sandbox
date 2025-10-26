#include "uniform.h"

#include <string.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <cglm/vec4.h>

#include "core.h"

CameraUniform *camera_uniform(Camera *camera) {
  return (CameraUniform *)camera->ubo_slot.uniform;
}

void camera_uniform_update(Camera *camera) {
  // Combine directly view matrix and camera position so faster to upload into
  // buffer

  CameraUniform *uniform = (CameraUniform *)camera->ubo_slot.uniform;

  // transform vec3 to vec4 for alignment sake
  vec4 pos_uniform = {
      camera->position[0],
      camera->position[1],
      camera->position[2],
      1.0f,
  };
  vec4 lookat_uniform = {
      camera->target[0],
      camera->target[1],
      camera->target[2],
      1.0f,
  };

  glm_vec4_copy(pos_uniform, uniform->position);
  glm_vec4_copy(lookat_uniform, uniform->lookat);
  glm_mat4_copy(camera->view, uniform->view);
  uniform->mode = camera->mode;

}

void camera_uniform_update_matrix_callback(void *callback_camera, void *data) {

  Camera *cast_cam = (Camera *)callback_camera;

  CameraUniform *updated_data = (CameraUniform *)cast_cam->ubo_slot.uniform;
  CameraUniform *new_data = (CameraUniform *)data;

  //  transfer updated camera values (position and view) to new data
  glm_mat4_copy(updated_data->view, new_data->view);
  glm_vec4_copy(updated_data->position, new_data->position);
  glm_vec4_copy(updated_data->lookat, new_data->lookat);
  new_data->mode = updated_data->mode;
}

/**
   Compare the callback camera new view with the last one to determine if the
   callback shall trigger
 */
bool camera_uniform_compare_views_callback(void *callback_camera,
                                           const void *entry_data) {

  // cast void* to camera*
  Camera *cast_cam = (Camera *)callback_camera;
  CameraUniform *cast_uni = (CameraUniform *)entry_data;

  // printf("compare: %d \n", memcmp(cast_cam->view, cast_uni->view,
  // sizeof(mat4)));

  // compare two views
  return memcmp(cast_cam->view, cast_uni->view, sizeof(mat4)) != 0;
}
