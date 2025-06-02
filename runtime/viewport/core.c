#include "core.h"
#include "../utils/system.h"
#include <cglm/cglm.h>

Viewport viewport_create(const ViewportCreateDescriptor *view_desc) {
  // set viewport default values
  Viewport v = {
      .fov = view_desc->fov,
      .near_clip = view_desc->near_clip,
      .far_clip = view_desc->far_clip,
      .aspect = view_desc->aspect,
      .clock = view_desc->clock,
  };

  // init projection matrix
  viewport_update_projection(&v);

  return v;
}

void viewport_update_projection(Viewport *viewport) {

  // update projection matrix
  float fov = glm_rad(viewport->fov);
  float far = viewport->far_clip;
  float near = viewport->near_clip;
  float aspect = viewport->aspect;
  float f = 1.0 / tan(fov * 0.5f);

  glm_perspective(fov, aspect, near, far, viewport->projection);
}

ViewportUniform viewport_uniform(Viewport *viewport) {
  ViewportUniform uViewport;
  glm_mat4_copy(viewport->projection, uViewport.projection);
  return uViewport;
}

mat4 *viewport_projection(Viewport *vp) { return &vp->projection; }
