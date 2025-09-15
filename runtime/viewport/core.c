#include "core.h"
#include "string.h"
#include <cglm/cglm.h>

#include "../utils/system.h"

void viewport_create(Viewport *viewport,
                     const ViewportCreateDescriptor *view_desc) {
  // set viewport default values
  viewport->fov = view_desc->fov;
  viewport->near_clip = view_desc->near_clip;
  viewport->far_clip = view_desc->far_clip;
  viewport->aspect = view_desc->aspect;
  viewport->width = view_desc->width;
  viewport->height = view_desc->height;

  // may be overriden/free by SSBO later when added to scene
  ssbo_slot_init_alloc(&viewport->ssbo_slot, sizeof(ViewportUniform));

  // init projection matrix
  viewport_update_projection(viewport);
  viewport_uniform_update(viewport);
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

ViewportUniform *viewport_uniform(Viewport *viewport) {
  return viewport->ssbo_slot.uniform;
}

void viewport_uniform_update(Viewport *viewport) {

  ViewportUniform *uniform = (ViewportUniform *)viewport->ssbo_slot.uniform;

  uniform->width = viewport->width, uniform->height = viewport->height,

  glm_mat4_copy(viewport->projection, uniform->projection);
}

mat4 *viewport_projection(Viewport *vp) { return &vp->projection; }

void viewport_destroy(Viewport *vp) {

  // means hasn't been assigned in the ssbo
  if (vp->ssbo_slot.id == SSBO_INDEX_UNFOUND)
    free(vp->ssbo_slot.uniform);

  memset(vp, 0, sizeof(Viewport));
}
