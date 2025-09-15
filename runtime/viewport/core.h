#ifndef _VIEWPORT_CORE_H_
#define _VIEWPORT_CORE_H_

#include <cglm/cglm.h>
#include <cglm/types.h>
#include <stdint.h>

#include "backend/clock.h"
#include "backend/ssbo.h"

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
  
  SSBOSlot ssbo_slot;
  
} Viewport;

void viewport_create(Viewport *, const ViewportCreateDescriptor *);
void viewport_update_projection(Viewport *);

void viewport_uniform_update(Viewport *);
ViewportUniform *viewport_uniform(Viewport *);

mat4 *viewport_projection(Viewport *);

void viewport_destroy(Viewport *);

#endif
