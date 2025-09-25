#ifndef SCENE_ENVIRONMENT_FOG_H_
#define SCENE_ENVIRONMENT_FOG_H_

#include "utils/color.h"

typedef struct {
  color color;
  float start_distance;
  float density;
} SceneEnvironmentFog;

typedef struct {
  color color;
  float start_distance;
  float density;
  vec2 _pad;
} SceneEnvironmentFogUniform;

#endif
