#include "draw.h"

#include "core.h"
#include "../runtime/mesh/core.h"

/**
   The function handles the dynamic change for draw mode.
 */

static const MeshShader map_shader_draw_mode[SCENE_RENDERER_DRAW_MODE_COUNT] = {
    [SceneRendererDrawMode_Texture] = MeshShader_Texture,
    [SceneRendererDrawMode_Solid] = MeshShader_Solid,
    [SceneRendererDrawMode_Wireframe] = MeshShader_Wireframe,
    [SceneRendererDrawMode_Boundbox] = MeshShader_Wireframe,
};

void scene_set_draw_mode(Scene *scene, const SceneRendererDrawMode mode) {

  // update active mesh shader depending on draw mode

  // update renderer drawn render pass configuration
  scene_renderer_set_draw_mode(&scene->renderer, mode);
}

