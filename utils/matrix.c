#include "matrix.h"
#include "string.h"
#include "system.h"

void matrix_lookat(UtilsMatrixLookatDescriptor *desc) {

  // update camera view
  // TODO : look like glm_..._copy is flawed
  // Can be related to alignment issue
  // Try with : alignas(16) mat4 view;
  memcpy(*desc->dest_position, desc->position, sizeof(vec3));

  vec3 *forward = desc->forward;
  vec3 *up = desc->up;
  vec3 *right = desc->right;

  vec3 adjusted_up = (vec3){0.0f, 1.0f, 0.0f};
  glm_vec3_sub(desc->target, desc->position, *forward);
  glm_normalize(*forward);

  // need to avoid forward being parallel to world_up
  // use x axis as up instead
  if (fabs(glm_vec3_dot(*forward, *up)) > 0.99f)
    glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, adjusted_up);

  // calculate right vector
  glm_vec3_cross(*up, *forward, *right);
  glm_normalize(*right);

  glm_lookat(*desc->dest_position, desc->target, adjusted_up,
             *desc->dest_matrix);
}

// TODO: unify lookat
void matrix_mesh_lookat(UtilsMatrixLookatDescriptor *desc) {

  // update dest position
  memcpy(*desc->dest_position, desc->position, sizeof(vec3));

  // calculate forward
  vec3 *forward =
      desc->forward == NULL ? &(vec3){0.0f, 0.0f, 0.0f} : desc->forward;
  glm_vec3_sub(desc->target, desc->position, *forward);
  glm_normalize(*forward);

  // calculate up
  vec3 *up = desc->up == NULL ? &(vec3){0.0f, 0.0f, 1.0f} : desc->up;
  if (fabs(glm_vec3_dot(*forward, *up)) > 0.99f)
    glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, *up);

  // calculate right
  vec3 *right = desc->right == NULL ? &(vec3){0.0f, 0.0f, 0.0f} : desc->right;
  glm_vec3_cross(*up, *forward, *right);
  glm_normalize(*right);

  // recompute up
  glm_vec3_cross(*forward, *right, *up);

  mat4 *model = desc->dest_matrix;

  glm_lookat(desc->position, desc->target, *up, *model);
  glm_mat4_inv(*model, *model); // invert the view matrix to get model matrix
}
