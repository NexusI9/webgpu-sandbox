//  RESOURCES
//  https://github.com/seyhajin/webgpu-wasm-c
//  https://developer.chrome.com/docs/web-platform/webgpu/build-app?hl=en
//  https://stackoverflow.com/questions/23997312/how-do-i-read-a-user-specified-file-in-an-emscripten-compiled-library

#include "backend/renderer/renderer.h"
#include "backend/renderer/scene/ao_bake/core.h"
#include "backend/renderer/scene/core.h"
#include "backend/renderer/scene/std_pipeline/layouts/layout.glass.h"
#include "resources/example/example.h"
#include <emscripten/emscripten.h>

// runtime
#include "resources/example/light.h"
#include "resources/example/primitive.h"
#include "resources/example/skybox.h"
#include "runtime/light/list.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/core.h"
#include "runtime/prefab/environment/skybox.h"
#include "runtime/primitive/core.h"
#include "runtime/scene/core.h"
#include "runtime/scene/draw.h"
#include "runtime/shader/update.h"
#include "runtime/texture/create.h"
#include "stdlib.h"

#include "./runtime/mesh/shader/shader.h"
#include "webgpu/webgpu.h"

static Scene main_scene;

// callback
static void init_scene();

void init_scene() {

  scene_create(&main_scene,
               &(SceneCreateDescriptor){
                   .renderer =
                       &(SceneRendererCreateDescriptor){
                           .name = "canvas",
                           .multisampling_count = PipelineMultisampleCount_4x,
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
               });

  example_light(&main_scene);
  example_skybox(&main_scene);
}

void on_camera_raycast(CameraRaycastCallback *cast_data, void *user_data) {
  printf("hover\n");
}

int main(int argc, const char *argv[]) {
  (void)argc, (void)argv; // unused

  // set scene
  init_scene();

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

  scene_set_draw_mode(&main_scene, SceneRendererDrawMode_Texture);

  // example_gltf(&main_scene);
  // example_ao(&main_scene, false);

  Mesh *cube = scene_new_mesh(&main_scene);
  example_primitive(cube, (vec3){0.0f, 0.0f, 0.0f}, &main_scene,
                    std_pipeline(PipelineType_Glass));

  // printf("mesh shader texture: %p\n", mesh_shader_texture(cube));
  shader_update_uniform(mesh_shader_texture(cube), 0, 3,
                        &(GlassUniform){
                            .color = {1.0f, 0.5f, 1.0f, 0.7f},
                            .roughness = 0.5f,
                        });

  WGPUTexture reflection;
  /*texture_create_cubemap_from_file(
      &reflection,
      &(TextureCreateCubeMapDescriptor){
          .resolution = 512,
          .path =
              &(CubeMapPath){
                  .right = "./resources/assets/texture/skybox/lake/right.png",
                  .left = "./resources/assets/texture/skybox/lake/left.png",
                  .top = "./resources/assets/texture/skybox/lake/top.png",
                  .bottom = "./resources/assets/texture/skybox/lake/bottom.png",
                  .front = "./resources/assets/texture/skybox/lake/front.png",
                  .back = "./resources/assets/texture/skybox/lake/back.png",
              },
      });*/

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
