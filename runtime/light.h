#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "../include/cglm/vec3.h"

typedef struct {
  vec3 position;
  vec4 color;
  float strength;
} PointLight;

typedef struct {
  vec4 color;
  float strength;
  vec3 _padding;
} AmbientLight;

typedef struct {
  vec3 position;
  vec3 target;
  vec4 color;
  vec2 _padding;
} DirectionalLight;




#endif
