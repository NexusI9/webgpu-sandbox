
//  RESOURCES
//  https://github.com/seyhajin/webgpu-wasm-c
//  https://developer.chrome.com/docs/web-platform/webgpu/build-app?hl=en
//  https://stackoverflow.com/questions/23997312/how-do-i-read-a-user-specified-file-in-an-emscripten-compiled-library

#include <stdbool.h>
#include <webgpu/webgpu.h>

// runtime
#include "backend/context.h"
#include "resources/example/glass.h"
#include "resources/example/gltf.h"
#include "resources/example/light.h"
#include "resources/example/skybox.h"
#include "runtime/input/core.h"
#include "runtime/pipeline/render.h"
#include "runtime/scene/core.h"
#include "runtime/scene/draw.h"
#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/renderer/core.h"
#include "runtime/texture/core.h"
#include "runtime/viewport/core.h"

static Scene main_scene;

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

  scene_create(&main_scene,
               &(SceneCreateDescriptor){
                   .renderer =
                       &(SceneRendererCreateDescriptor){
                           .background = (WGPUColor){0.1f, 0.1f, 0.1f, 1.0f},
                           .dpi = 1.0,
                       },
                   .viewport =
                       &(ViewportCreateDescriptor){
                           .fov = 32.0f,
                           .near_clip = 0.1f,
                           .far_clip = 100.0f,
                           .aspect = 16.0f / 9.0f,
                       },
                   .ui =
                       &(SceneEditorUIConfig){
                           .dpi = 2.0,
                       },
               });

  scene_set_draw_mode(&main_scene, SceneRendererDrawMode_Solid);

  example_light(&main_scene);
  example_skybox(&main_scene);
  example_gltf_podium(&main_scene);


  // example_ao(&main_scene, true);
  // example_glass_box(&main_scene);
  // example_glass_probe_grid(&main_scene, false);
  example_glass_probe_plane(&main_scene, false);

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
  scene_renderer_draw(&main_scene.renderer);

  // Quit
  scene_renderer_close(&main_scene.renderer);

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
