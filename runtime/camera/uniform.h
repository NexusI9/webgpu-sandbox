#ifndef _CAMERA_UNIFORM_H_
#define _CAMERA_UNIFORM_H_

#include "core.h"

typedef struct {
  mat4 view;
  vec4 position;
  vec4 lookat;
  uint32_t mode;
  uint32_t _pad[4];
} __attribute__((aligned(16))) CameraUniform;

CameraUniform camera_uniform(Camera *);
void camera_uniform_update_matrix(void *, void *);
bool camera_uniform_compare_views(void *, const void*);

#endif
