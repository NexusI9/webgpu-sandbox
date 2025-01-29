#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include "../include/cglm/mat4.h"

typedef struct {
  float fov;
  float near_clip;
  float far_clip;
  float aspect;
} ViewportCreateDescriptor;

typedef struct {
  float fov;
  float near_clip;
  float far_clip;
  float aspect;
  mat4 projection;
} viewport;

viewport viewport_create(const ViewportCreateDescriptor *);
mat4 *viewport_projection_matrix(viewport *);

#endif
