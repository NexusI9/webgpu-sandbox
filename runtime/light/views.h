#ifndef _LIGHT_VIEWS_H_
#define _LIGHT_VIEWS_H_
#include "core.h"
#include <cglm/cglm.h>

typedef struct {
  mat4 views[LIGHT_POINT_VIEWS];
  uint8_t length;
} LightViews;

// projections/view computing
void light_point_views(LightViews *, vec3, float, float);
void light_spot_view(LightViews *, vec3, vec3, float);
void light_sun_view(LightViews *, vec3, float);

#endif
