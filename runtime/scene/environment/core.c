#include "core.h"
#include "../backend/std_texture/std_texture.h"
#include "fog.h"

void scene_environment_init(SceneEnvironment *env,
                            const SceneEnvironmentDescriptor *desc) {

  env->ssbo = desc->ssbo;
  env->ubo = desc->ubo;

  // init default skybox
  env->skybox.view = std_texture_view(TextureViewType_FloatCube);

  // init fog
  scene_environment_set_fog(env, &(SceneEnvironmentFog){
                                     .color = {1.0f, 1.0f, 1.0f, 0.0f},
                                     .density = 600.0f,
                                     .start_distance = 80.0f,
                                 });
}

void scene_environment_set_fog(SceneEnvironment *env,
                               const SceneEnvironmentFog *settings) {

  env->fog = *settings;

  SceneEnvironmentFogUniform uniform = {
      .density = env->fog.density,
      .start_distance = env->fog.start_distance,
  };
  glm_vec4_copy(env->fog.color, uniform.color);

  ubo_update_entry(env->ubo, UBOField_Fog, (void *)&uniform);
  ubo_upload(env->ubo);
}

SceneEnvironmentSkybox *scene_environment_skybox(SceneEnvironment *env) {
  return &env->skybox;
}
