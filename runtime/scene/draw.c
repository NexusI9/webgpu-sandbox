#include "draw.h"

#include "core.h"
#include "runtime/light/shadow_map/draw.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/build.h"
#include "runtime/scene/renderer/core.h"
#include <stdint.h>

static const ScenePipeline scene_dynamic_pipelines[4] = {
    ScenePipeline_Dynamic_Lit,
    ScenePipeline_Dynamic_LitShadow,
    ScenePipeline_Dynamic_Lit,
    ScenePipeline_Dynamic_Unlit,
};

void scene_set_draw_mode(Scene *scene, const SceneRendererDrawMode mode) {

  if (mode == scene->renderer.draw.mode)
    return;

  // build dynamic meshes
  for (uint8_t i = 0; i < 4; i++)
    scene_build_mesh_ref_list(scene,
                              scene_pipeline(scene, scene_dynamic_pipelines[i]),
                              scene_dynamic_pipelines[i], mode);

  // update light / reflections
  if (mode == SceneRendererDrawMode_Texture) {

    shadow_map_draw_all(
        &(ShadowMapDrawAllDescriptor){
            .mesh_list = scene_pipeline(scene, ScenePipeline_Dynamic_LitShadow),
            .lights = &scene->lights,
        },
        SCENE_DEBUG_UNDEFINED);
  }

  // update renderer drawn render pass configuration
  scene_renderer_set_draw_mode(&scene->renderer, mode);
}
