#ifndef _LIGHT_VIEWS_H_
#define _LIGHT_VIEWS_H_
#include "core.h"
#include <cglm/cglm.h>

typedef struct {
  mat4 views[LIGHT_POINT_VIEWS];
  uint8_t length;
} LightViews;

// projections/view computing
LightViews light_point_views(vec3, float, float);
LightViews light_spot_view(vec3, vec3, float);
LightViews light_sun_view(vec3, float);

#endif
