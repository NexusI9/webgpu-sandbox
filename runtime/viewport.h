#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include "../backend/clock.h"
#include "../include/cglm/mat4.h"

typedef struct {
  mat4 projection;
} ViewportUniform;

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
} viewport;

viewport viewport_create(const ViewportCreateDescriptor *);
void viewport_update_projection(viewport *);
ViewportUniform viewport_uniform(viewport *);

#endif
