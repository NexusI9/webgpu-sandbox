#include "draw.h"

#include "core.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/build.h"
#include <stdint.h>

static const ScenePipeline scene_dynamic_pipelines[3] = {
    ScenePipeline_Dynamic_Lit,
    ScenePipeline_Dynamic_LitShadow,
    ScenePipeline_Dynamic_Lit,
};

void scene_set_draw_mode(Scene *scene, const SceneRendererDrawMode mode) {

  if (mode == scene->renderer.draw.mode)
    return;

  // build dynamic meshes
  for (uint8_t i = 0; i < 3; i++)
    scene_build_mesh_ref_list(scene,
                              scene_pipeline(scene, scene_dynamic_pipelines[i]),
                              scene_dynamic_pipelines[i], mode);

  // update renderer drawn render pass configuration
  scene_renderer_set_draw_mode(&scene->renderer, mode);
}
