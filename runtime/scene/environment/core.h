#ifndef SCENE_ENVIRONMENT_CORE_H_
#define SCENE_ENVIRONMENT_CORE_H_

#include "../utils/color.h"
#include <webgpu/webgpu.h>

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

typedef struct {
  WGPUTexture texture;
  WGPUTextureView view;
} SceneEnvironmentSkybox;

typedef struct {
  SceneEnvironmentFog fog;
  SceneEnvironmentSkybox skybox;
} SceneEnvironment;

void scene_environment_init(SceneEnvironment *);

void scene_environment_set_fog(SceneEnvironment *, const SceneEnvironmentFog *);
void scene_environment_set_skybox(SceneEnvironment *, WGPUTexture,
                                  WGPUTextureView);

SceneEnvironmentSkybox *scene_environment_skybox(SceneEnvironment *);
#endif
