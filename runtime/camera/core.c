#include "core.h"

#include <stddef.h>
#include <cglm/mat4.h>
#include <cglm/vec3.h>

#include "../../utils/matrix.h"
#include "./uniform.h"
#include "math.h"
#include "../backend/registry.h"
#include "../backend/ssbo.h"

void camera_create(Camera *cam, const CameraCreateDescriptor *cd) {

  cam->id = reg_register((void *)cam, RegEntryType_Camera);

  // set matrix and position to 0
  camera_reset(cam);

  // assign additional attributes
  cam->speed = cd->speed;
  cam->clock = cd->clock;
  cam->mode = cd->mode;
  cam->sensitivity = cd->sensitivity;

  // may be overriden/free by SSBO later when added to scene
  ssbo_slot_init_alloc(&cam->ssbo_slot, sizeof(CameraUniform));
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

    c->ssbo_slot.uniform = NULL;
  }
}

void camera_set_position(Camera *camera, vec3 new_position) {
  // get the absolute value, need to transfom the new position into
  // the camera coordinate system (relative)
  // https://www.ogldev.org/www/tutorial13/tutorial13.html

  camera->position[0] += new_position[0];
  camera->position[1] += new_position[1];
  camera->position[2] += new_position[2];

  camera_update_view(camera);
  camera_uniform_update(camera);
}

void camera_set_rotation(Camera *camera, vec3 new_rotation) {
  glm_vec3_copy(new_rotation, camera->euler_rotation);

  camera_update_view(camera);
  camera_uniform_update(camera);
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

void camera_lookat(Camera *camera, vec3 position, vec3 target) {

  matrix_lookat(&(UtilsMatrixLookatDescriptor){
      .dest_position = &camera->position,
      .dest_matrix = &camera->view,
      .forward = &camera->forward,
      .right = &camera->right,
      .up = &camera->up,
      .position = position,
      .target = target,
  });

  camera_uniform_update(camera);
}

mat4 *camera_view(Camera *camera) { return &camera->view; }
