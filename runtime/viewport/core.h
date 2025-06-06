#ifndef _VIEWPORT_CORE_H_
#define _VIEWPORT_CORE_H_

#include "../backend/clock.h"
#include <cglm/cglm.h>

typedef struct {
  mat4 projection;
  uint32_t width;
  uint32_t height;
} __attribute__((aligned(16))) ViewportUniform;

typedef struct {
  float fov;
  float near_clip;
  float far_clip;
  float aspect;
  uint32_t width;
  uint32_t height;
  cclock *clock;
} ViewportCreateDescriptor;

typedef struct {
  float fov;
  float near_clip;
  float far_clip;
  uint32_t width;
  uint32_t height;
  float aspect;
  mat4 projection;
  cclock *clock;
} Viewport;

void viewport_create(Viewport *, const ViewportCreateDescriptor *);
void viewport_update_projection(Viewport *);
ViewportUniform viewport_uniform(Viewport *);
mat4 *viewport_projection(Viewport *);

#endif
