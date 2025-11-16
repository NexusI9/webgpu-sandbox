#include "core.h"

#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <cglm/util.h>
#include <math.h>
#include <stdlib.h>

#include "backend/ubo.h"
#include "string.h"

void viewport_create(Viewport *viewport,
                     const ViewportCreateDescriptor *view_desc) {
  // set viewport default values
  viewport->fov = view_desc->fov;
  viewport->near_clip = view_desc->near_clip;
  viewport->far_clip = view_desc->far_clip;
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
  float aspect = (float)viewport->width / (float)viewport->height;

  float f = 1.0 / tan(fov * 0.5f);

  glm_perspective(fov, aspect, near, far, viewport->projection);
}

ViewportUniform *viewport_uniform(Viewport *viewport) {
  return viewport->ubo_slot.uniform;
}

void viewport_uniform_update(Viewport *viewport) {

  ViewportUniform *uniform = (ViewportUniform *)viewport->ubo_slot.uniform;

  uniform->width = viewport->width;
  uniform->height = viewport->height;
  viewport_update_projection(viewport);

  glm_mat4_copy(viewport->projection, uniform->projection);
}

mat4 *viewport_projection(Viewport *vp) { return &vp->projection; }

void viewport_destroy(Viewport *vp) {

  // means hasn't been assigned in the ubo
  if (vp->ubo_slot.id == UBO_INDEX_UNFOUND)
    free(vp->ubo_slot.uniform);

  memset(vp, 0, sizeof(Viewport));
}
