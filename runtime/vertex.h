#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "../include/cglm/vec2.h"
#include "../include/cglm/vec3.h"

typedef struct {

  vec3 position;
  vec3 normal;
  vec3 color;
  vec2 uv;

} vertex;

void vertex_create(vertex *);

#endif
