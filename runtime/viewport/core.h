#ifndef _VIEWPORT_CORE_H_
#define _VIEWPORT_CORE_H_

#include "../backend/clock.h"
#include <cglm/cglm.h>

typedef struct {
  mat4 projection;
} __attribute__((aligned(16))) ViewportUniform;

typedef struct {
  float fov;
  float near_clip;
  float far_clip;
  float aspect;
  cclock *clock;
} ViewportCreateDescriptor;

typedef struct {
  float fov;
  float near_clip;
  float far_clip;
  float aspect;
  mat4 projection;
  cclock *clock;
} Viewport;

Viewport viewport_create(const ViewportCreateDescriptor *);
void viewport_update_projection(Viewport *);
ViewportUniform viewport_uniform(Viewport *);
mat4 *viewport_projection(Viewport *);

#endif
