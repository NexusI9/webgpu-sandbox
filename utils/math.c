#include "math.h"
#include "stdio.h"
#include "system.h"

void perspective_lh(float fov, float aspect, float near, float far,
                    mat4 destination) {
  float f = 1.0 / tan(fov * 0.5f);

  // left handed coordinate system
  mat4 proj = {
      {f / aspect, 0.0f, 0.0f, 0.0f},
      {0.0f, f, 0.0f, 0.0f},
      {0.0f, 0.0f, far / (far - near), 1.0f},
      {0.0f, 0.0f, -1.0f * (near * far) / (far - near), 0.0f},
  };

  // replace viewport projection
  glm_mat4_copy(proj, destination);
}

/**
   Look at for left handed coordinate system
   Also provide to update forward, and right vector
   (used for Camera)
 */
void lookat_lh(vec3 eye, vec3 center, vec3 up, mat4 destination,
               vec3 dest_forward, vec3 dest_right) {

  vec3 forward, right, up_adjusted;

  glm_vec3_sub(center, eye, forward);
  glm_normalize(forward);
  if (dest_forward)
    glm_vec3_copy(forward, dest_forward);

  // need to avoid forward being parallel to world_up
  // use x axis as up instead
  // if (fabs(glm_vec3_dot(forward, up)) > 0.99f)
  //  glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, up);

  // calculate right vector
  glm_vec3_cross(up, forward, right);
  glm_normalize(right);
  if (dest_right)
    glm_vec3_copy(right, dest_right);

  // calculate up vector
  glm_vec3_cross(forward, right, up_adjusted);

  // create new view matrix
  mat4 view = {
      {
          right[0],
          up_adjusted[0],
          -forward[0],
          0.0f,
      },
      {
          right[1],
          up_adjusted[1],
          -forward[1],
          0.0f,
      },
      {
          right[2],
          up_adjusted[2],
          -forward[2],
          0.0f,
      },
      {
          -glm_vec3_dot(right, eye),
          -glm_vec3_dot(up_adjusted, eye),
          glm_vec3_dot(forward, eye),
          1.0f,
      },
  };

  glm_mat4_copy(view, destination);
}
