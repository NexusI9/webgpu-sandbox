#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_
#include "vertex.h"
#include <stdint.h>

typedef struct {
  vertex a;
  vertex b;
  vertex c;
} triangle;

void triangle_random_points(triangle *, uint16_t, vec3 *);
void triangle_normal(triangle *, vec3);
void triangle_raycast(triangle *, vec3, vec3, float, vec3);
void triangle_point_to_uv(triangle *, vec3, vec2);
void triangle_center(triangle*, vec3);

#endif
