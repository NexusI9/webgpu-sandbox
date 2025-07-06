#include "core.h"
#include "../input/input.h"
#include <float.h>

bool raycast_hit_aabb(Raycast *ray, const AABB *box, float* distance) {

  
  float tmin = -FLT_MAX, tmax = FLT_MAX;

  for (int i = 0; i < 3; ++i) {

    if (fabs(ray->direction[i]) < 1e-6) {
      // ray parallel to slab
      if (ray->origin[i] < box->min[i] || ray->origin[i] > box->max[i])
        return false;
    } else {
      float inv_d = 1.0f / ray->direction[i];
      float t1 = (box->min[i] - ray->origin[i]) * inv_d;
      float t2 = (box->max[i] - ray->origin[i]) * inv_d;

      if (t1 > t2) {
        float tmp = t1;
        t1 = t2;
        t2 = tmp;
      }

      if (t1 > tmin)
        tmin = t1;

      if (t2 < tmax)
        tmax = t2;

      if (tmin > tmax)
        return false;
    }
  }

  if(distance) *distance = tmin;
  return true;
}
