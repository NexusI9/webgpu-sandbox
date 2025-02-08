#include "viewport.h"
#include <math.h>

viewport viewport_create(const ViewportCreateDescriptor *view_desc) {
  // set viewport default values
  viewport v = (viewport){
      .fov = view_desc->fov,
      .near_clip = view_desc->near_clip,
      .far_clip = view_desc->far_clip,
      .aspect = view_desc->aspect,
  };

  // init projection matrix
  viewport_update_projection(&v);

  return v;
}

void viewport_update_projection(viewport *viewport) {

  // update projection matrix
  /*  float fov = glm_rad(viewport->fov);
    float far = viewport->far_clip;
    float near = viewport->near_clip;
    float aspect = viewport->aspect;
    float f = 1.0 / tan(fov * 0.5f);
  */

  float fov = glm_rad(24.0f);
  float far = 100.0f;
  float near = 0.1f;
  float aspect = 1920.0f / 1080.0f;
  float f = 1.0f / tan(fov * 0.5f);

  mat4 proj = {
      {f / aspect, 0.0f, 0.0f, 0.0f},
      {0.0f, f, 0.0f, 0.0f},
      {0.0f, 0.0f, far / (far - near), 1.0f},
      {0.0f, 0.0f, -1.0f * (near * far) / (far - near), 0.0f},
  };

  // replace viewport projection
  glm_mat4_copy(proj, viewport->projection);
}
