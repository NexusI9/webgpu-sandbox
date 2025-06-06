#include "core.h"
#include "../utils/system.h"
#include <cglm/cglm.h>

void viewport_create(Viewport *viewport,
                     const ViewportCreateDescriptor *view_desc) {
  // set viewport default values
  viewport->fov = view_desc->fov, viewport->near_clip = view_desc->near_clip,
  viewport->far_clip = view_desc->far_clip,
  viewport->aspect = view_desc->aspect, viewport->clock = view_desc->clock,
  viewport->width = view_desc->width;
  viewport->height = view_desc->height;

  // init projection matrix
  viewport_update_projection(viewport);
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
  ViewportUniform uViewport = {
      .width = viewport->width,
      .height = viewport->height,
  };

  glm_mat4_copy(viewport->projection, uViewport.projection);
  return uViewport;
}

mat4 *viewport_projection(Viewport *vp) { return &vp->projection; }
