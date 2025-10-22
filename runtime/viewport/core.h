#ifndef _VIEWPORT_CORE_H_
#define _VIEWPORT_CORE_H_

#include <cglm/cglm.h>
#include <cglm/types.h>
#include <stdint.h>

#include "backend/clock.h"
#include "backend/ssbo.h"
#include "utils/defines.h"

typedef struct {
  mat4 projection;
  uint32_t width;
  uint32_t height;
  uint32_t _pad[46];
} ViewportUniform;

typedef struct {
  float fov;
  float near_clip;
  float far_clip;
  int width;
  int height;
  cclock *clock;
} ViewportCreateDescriptor;

typedef struct {

  float fov;
  float near_clip;
  float far_clip;
  int width;
  int height;
  mat4 projection;

  SSBOSlot ssbo_slot;

} Viewport;

EXTERN_C_BEGIN

void viewport_create(Viewport *, const ViewportCreateDescriptor *);
void viewport_update_projection(Viewport *);

void viewport_uniform_update(Viewport *);
ViewportUniform *viewport_uniform(Viewport *);

mat4 *viewport_projection(Viewport *);

void viewport_destroy(Viewport *);

static inline float viewport_fov(Viewport *viewport) { return viewport->fov; }
static inline float viewport_width(Viewport *viewport) {
  return viewport->width;
}
static inline float viewport_height(Viewport *viewport) {
  return viewport->height;
}

static inline float viewport_near_clip(Viewport *viewport) {
  return viewport->near_clip;
}
static inline float viewport_far_clip(Viewport *viewport) {
  return viewport->far_clip;
}

static inline void viewport_set_fov(Viewport *viewport, const float value) {
  viewport->fov = value;
}

static inline void viewport_set_near_clip(Viewport *viewport,
                                          const float value) {
  viewport->near_clip = value;
}

static inline void viewport_set_far_clip(Viewport *viewport,
                                         const float value) {
  viewport->far_clip = value;
}

static inline void viewport_set_width(Viewport *viewport, const float value) {
  viewport->width = value;
}

static inline void viewport_set_height(Viewport *viewport, const float value) {
  viewport->height = value;
}

EXTERN_C_END

#endif
