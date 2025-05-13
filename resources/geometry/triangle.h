#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_
#include "vertex.h"
#include <stdint.h>

typedef struct {
  Vertex a;
  Vertex b;
  Vertex c;
} Triangle;

void triangle_random_points(Triangle *, uint16_t, vec3 *);
void triangle_normal(Triangle *, vec3);
void triangle_raycast(Triangle *, vec3, vec3, float, vec3);
void triangle_point_to_uv(Triangle *, vec3, vec2);
void triangle_center(Triangle *, vec3);
#endif
