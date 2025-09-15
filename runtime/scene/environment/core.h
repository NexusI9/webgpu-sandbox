#ifndef SCENE_ENVIRONMENT_CORE_H_
#define SCENE_ENVIRONMENT_CORE_H_

#include <webgpu/webgpu.h>

#include "backend/ssbo.h"
#include "backend/ubo.h"
#include "./fog.h"
#include "utils/color.h"
#include "fog.h"

typedef struct {
  WGPUTexture texture;
  WGPUTextureView view;
} SceneEnvironmentSkybox;

typedef struct {
  UBOManager *ubo;
  SSBOManager *ssbo;
  SceneEnvironmentFog fog;
  SceneEnvironmentSkybox skybox;
} SceneEnvironment;

typedef struct {
  UBOManager *ubo;
  SSBOManager *ssbo;
} SceneEnvironmentDescriptor;

void scene_environment_init(SceneEnvironment *,
                            const SceneEnvironmentDescriptor *);

void scene_environment_set_fog(SceneEnvironment *, const SceneEnvironmentFog *);
void scene_environment_set_skybox(SceneEnvironment *, WGPUTexture,
                                  WGPUTextureView);

SceneEnvironmentSkybox *scene_environment_skybox(SceneEnvironment *);
#endif
