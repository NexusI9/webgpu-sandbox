#ifndef SCENE_ENVIRONMENT_CORE_H_
#define SCENE_ENVIRONMENT_CORE_H_

#include <webgpu/webgpu.h>

#include "backend/ubo.h"
#include "runtime/light/list.h"
#include "runtime/probe/core.h"
#include "utils/color.h"

typedef struct {
  WGPUTexture texture;
  WGPUTextureView view;
} SceneEnvironmentSkybox;

typedef struct {
  color color;
  float start_distance;
  float density;
  float _pad[2];
} SceneEnvironmentFog;

typedef struct {
  SceneEnvironmentFog fog;
  SceneEnvironmentSkybox skybox;
  UBOSlot ubo_slot;
} SceneEnvironment;

typedef struct {
  SceneEnvironmentFog fog;
  float _pad[56];
} SceneEnvironmentUniform;

void scene_environment_init(SceneEnvironment *);

void scene_environment_set_skybox(SceneEnvironment *, WGPUTexture,
                                  WGPUTextureView);

SceneEnvironmentSkybox *scene_environment_skybox(SceneEnvironment *);

void scene_environment_update_uniform(SceneEnvironment *);

#endif
