#include "viewport.h"
#include <math.h>

viewport viewport_create(const ViewportCreateDescriptor * view_desc) {
  // set viewport default values
  viewport v =
      (viewport){
      .fov = view_desc->fov,
      .near_clip = view_desc->near_clip,
      .far_clip = view_desc->far_clip
  };

  // init projection matrix
  glm_mat4_identity(v.projection);

  return v;
}

mat4 *viewport_projection_matrix(viewport *viewport) {

  // update projection matrix
  float far = viewport->far_clip;
  float near = viewport->near_clip;
  float aspect = viewport->aspect;
  float f = 1.0 / tan(viewport->fov * 0.5f);
  mat4 proj = {
      {f / aspect, 0.0f, 0.0f, 0.0f},
      {0.0f, f, 0.0f, 0.0f},
      {0.0f, 0.0f, far / (far - near), 1.0f},
      {0.0f, 0.0f, -1.0f * (near * far) / (far - near), 0.0f},
  };

  // replace viewport projection
  glm_mat4_copy(proj, viewport->projection);
  return &viewport->projection;
}
