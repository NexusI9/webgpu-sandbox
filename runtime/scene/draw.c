#include "draw.h"
#include "../backend/renderer/renderer.h"
#include "../camera/camera.h"
#include "core.h"

/**
   DELETME
   Draw callback added to the Scene Renderer draw callbacks.
   Called before the scene renderer draw layouts.

   Basically udpate the camera matrix based on its mode and user input.
 */
void scene_camera_draw_callback(void *data) {

}

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

/**
   Update textures of all certain type of lights (Point, Sun, Spot...)
   Function used when an object is added to the scene.
 */
void scene_update_shadow_map(Scene *scene, const LightType light_type) {}
