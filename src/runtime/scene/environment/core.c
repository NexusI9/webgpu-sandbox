#include "core.h"

#include <cglm/vec4.h>

#include "backend/ubo.h"
#include "backend/std_texture/core.h"

void scene_environment_init(SceneEnvironment *env) {

  // init default skybox
  env->skybox.view = std_texture_view(TextureViewType_FloatCube);

  // init fog
  env->fog = (SceneEnvironmentFog){
      .color = {1.0f, 1.0f, 1.0f, 0.0f},
      .density = 600.0f,
      .start_distance = 80.0f,
  };

  ubo_slot_init_alloc(&env->ubo_slot, sizeof(SceneEnvironmentUniform));
  scene_environment_update_uniform(env);
}

SceneEnvironmentSkybox *scene_environment_skybox(SceneEnvironment *env) {
  return &env->skybox;
}

void scene_environment_update_uniform(SceneEnvironment *env) {

  SceneEnvironmentUniform *uniform =
      (SceneEnvironmentUniform *)env->ubo_slot.uniform;

  glm_vec4_copy(env->fog.color, uniform->fog.color);
  uniform->fog.density = env->fog.density;
  uniform->fog.start_distance = env->fog.start_distance;
}
