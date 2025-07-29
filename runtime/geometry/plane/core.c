#include "core.h"

void inf_plane_create(InfinitePlane *plane, vec3 origin, vec3 direction) {
  glm_vec3_copy(direction, plane->normal);
  glm_vec3_normalize(plane->normal);
  plane->d = -glm_vec3_dot(plane->normal, origin);
}
