
//  RESOURCES
//  https://github.com/seyhajin/webgpu-wasm-c
//  https://developer.chrome.com/docs/web-platform/webgpu/build-app?hl=en
//  https://stackoverflow.com/questions/23997312/how-do-i-read-a-user-specified-file-in-an-emscripten-compiled-library
//  https://bevy.org/learn/quick-start/getting-started/ecs/

#include <stdbool.h>
#include <webgpu/webgpu.h>

// runtime
#include "backend/context.h"
#include "backend/renderer/core.h"
#include "backend/resource_manager.h"
#include "backend/theme/core.h"
#include "backend/ubo.h"
#include "resources/example/glass.h"
#include "resources/example/gltf.h"
#include "resources/example/light.h"
#include "resources/example/skybox.h"
#include "runtime/engine/add.h"
#include "runtime/engine/core.h"
#include "runtime/gui/core.h"
#include "runtime/input/core.h"
#include "runtime/pipeline/render.h"
#include "runtime/scene/core.h"
#include "runtime/systems/scene_system.h"
#include "runtime/systems/selection_system.h"
#include "runtime/systems/ubo_system.h"
#include "runtime/texture/core.h"
#include "runtime/viewport/core.h"

int main(int argc, const char *argv[]) {

  (void)argc, (void)argv; // unused

  context_init(&(ContextDescriptor){
      .html_target = "canvas",
      .input =
          &(InputDescriptor){
              .mouse_sensitivity = 0.02f,
              .wheel_sensitivity = 0.02f,
          },
      .render =
          {
              .multisample_count = PipelineMultisampleCount_4x,
          },
  });

  Engine engine;
  engine_init(&engine);

  Scene *scene = engine_get_active_scene(&engine);

  // example_light(&main_scene);
  example_skybox(&engine);
  example_gltf_spa(&engine);

  // example_ao(&main_scene, true);
  // example_glass_box(&main_scene);
  // example_glass_probe_grid(&main_scene, false);
  // example_glass_probe_plane(&main_scene, false);

  // add gizmo camera
  /*GizmoCamera *new_cam =
      scene_add_camera(&main_scene, &(CameraCreateDescriptor){
                                        .speed = 20.0f,
                                        .clock = &main_clock,
                                        .mode = CameraMode_Fixed,
                                        .sensitivity = {0},
                                    });

  gizmo_camera_lookat(new_cam, (vec3){10.0f, 2.0f, 0.0f},
                      (vec3){0.0f, 0.0f, 0.0f});
   */

  // Update Loop
  renderer_draw(engine.renderer);

  return 0;
}

/*#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

// EMSCRIPTEN_KEEPALIVE make function available in web environment (not
// eliminated as DEAD code)
EXTERN EMSCRIPTEN_KEEPALIVE void setContext(int w, int h, int d) {
  state.context.width = w;
  state.context.height = h;
  state.context.dpi = d;

  printf("%d\t%d\t%d\n", w, h, d);
  }*/
