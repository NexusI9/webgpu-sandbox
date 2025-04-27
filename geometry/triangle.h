#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_
#include "vertex.h"
#include <stdint.h>

typedef struct {
  vertex a;
  vertex b;
  vertex c;
} triangle;

void triangle_random_points(triangle *, uint16_t, vec3*);

#endif
