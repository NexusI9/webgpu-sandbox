#include "core.h"
#include "../backend/renderer/scene/std_texture/std_texture.h"

void scene_environment_init(SceneEnvironment *env) {

  // init default skybox
  env->skybox.view = std_texture_view(TextureViewType_FloatCube);

  // init fog
  scene_environment_set_fog(env, &(SceneEnvironmentFog){
                                     .color = {1.0f, 1.0f, 1.0f, 0.0f},
                                     .density = 1000.0f,
                                     .start_distance = 80.0f,
                                 });
}

void scene_environment_set_fog(SceneEnvironment *env,
                               const SceneEnvironmentFog *settings) {

  env->fog = *settings;
}

SceneEnvironmentSkybox *scene_environment_skybox(SceneEnvironment *env) {
  return &env->skybox;
}
