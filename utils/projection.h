#ifndef _UTILS_VIEW_H_
#define _UTILS_VIEW_H_

#include <cglm/cglm.h>

#define PROJECTION_VIEW_COUNT 6
#define PROJECTION_SUN_DISTANCE 10

typedef struct {
  mat4 view;
  uint32_t _pad[48];
} ProjectionUniform;

typedef struct {
  mat4 projection;
  uint8_t length;
  mat4 views[PROJECTION_VIEW_COUNT];
  mat4 combined[PROJECTION_VIEW_COUNT];
} Projection;

// projections/view computing
void projection_point(Projection *, vec3, float, float);
void projection_spot(Projection *, vec3, vec3, float);
void projection_sun(Projection *, vec3, float);

#endif
