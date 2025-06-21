//  RESOURCES
//  https://github.com/seyhajin/webgpu-wasm-c
//  https://developer.chrome.com/docs/web-platform/webgpu/build-app?hl=en
//  https://stackoverflow.com/questions/23997312/how-do-i-read-a-user-specified-file-in-an-emscripten-compiled-library

#include <emscripten/emscripten.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "resources/example/example.h"

#include "backend/renderer.h"

// runtime
#include "runtime/camera/camera.h"
#include "runtime/camera/core.h"
#include "runtime/gizmo/core.h"
#include "runtime/gizmo/transform_translate.h"
#include "runtime/input/input.h"
#include "runtime/light/light.h"
#include "runtime/material/material.h"
#include "runtime/mesh/mesh.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/scene/core.h"
#include "runtime/scene/scene.h"
#include "runtime/shader/shader.h"
#include "runtime/viewport/viewport.h"

static Scene main_scene;
static Mesh tri_mesh;
static float rot = 0.0f;
static renderer main_renderer;
static cclock main_clock;

// callback
static void init_pipeline();
static void setup_triangle();
static void init_scene();

// dir light
static vec3 LIGHT_POSITION = {0.0f, 4.0f, 6.0f};
static vec3 LIGHT_TARGET = {0.0f, 0.0f, 0.0f};

// point light
static vec3 POINT_LIGHT = {0.0f, 2.4f, 2.3f};

// sun light
static vec3 SUN_LIGHT = {-2.0f, 2.0f, 2.0f};

void init_scene() {

  // set viewport
  Viewport viewport;
  viewport_create(&viewport, &(ViewportCreateDescriptor){
                                 .fov = 32.0f,
                                 .near_clip = 0.1f,
                                 .far_clip = 100.0f,
                                 .aspect = 16.0f / 9.0f,
                                 .width = renderer_width(&main_renderer),
                                 .height = renderer_height(&main_renderer),
                             });

  // set camera
  Camera camera;
  camera_create(&camera, &(CameraCreateDescriptor){
                             .speed = 20.0f,
                             .clock = &main_clock,
                             .mode = CameraMode_Flying,
                             .sensitivity = 0.2f,
                             .wheel_sensitivity = 0.01f,
                         });

  scene_create(&main_scene, camera, viewport);

  // init camera position
  camera_lookat(main_scene.active_camera,
                (vec3){
                    20.0f,
                    20.0f,
                    20.0f,
                },
                (vec3){
                    0.0f,
                    0.0f,
                    0.0f,
                });

  scene_add_sun_light(
      &main_scene,
      &(SunLightDescriptor){
          .position = {SUN_LIGHT[0], SUN_LIGHT[1], SUN_LIGHT[2]},
          .color = {1.0f, 1.0f, 1.0f},
          .intensity = 1.0f,
          .size = 10.0f,
      },
      renderer_device(&main_renderer), renderer_queue(&main_renderer));

  // set light
  // TODO: uniformise WGPUDevice/WGPUQueue opaque pointer passing (& || * ? )
  scene_add_point_light(&main_scene,
                        &(PointLightDescriptor){
                            .color = {1.0f, 0.0f, 0.3f},
                            .intensity = 4.0f,
                            .cutoff = 20.0f,
                            .inner_cutoff = 50.0f,
                            .near = 0.1,
                            .far = 20.0f,
                            .position =
                                {
                                    POINT_LIGHT[0],
                                    POINT_LIGHT[1],
                                    POINT_LIGHT[2],
                                },
                        },
                        renderer_device(&main_renderer),
                        renderer_queue(&main_renderer));

  /*scene_add_spot_light(&main_scene, &(SpotLightDescriptor){
                                        .color = {1.0f, 1.0f, 1.0f},
                                        .intensity = 2.0f,
                                        .cutoff = 45.0f,
                                        .angle = 45.0f,
                                        .inner_cutoff = 30.0f,
                                        .target =
                                            {
                                                LIGHT_TARGET[0],
                                                LIGHT_TARGET[1],
                                                LIGHT_TARGET[2],
                                            },
                                        .position =
                                            {
                                                LIGHT_POSITION[0],
                                                LIGHT_POSITION[1],
                                                LIGHT_POSITION[2],
                                            },
                                            });*/

  scene_add_ambient_light(&main_scene,
                          &(AmbientLightDescriptor){
                              .color = {1.0f, 1.0f, 1.0f},
                              .intensity = 0.2f,
                              .position = {2.0f, 4.0f, -5.0f},
                          },
                          renderer_device(&main_renderer),
                          renderer_queue(&main_renderer));
}

void add_grid() {
  GridUniform grid_uniform = {
      .size = 100.0f,
      .cell_size = 100.0f,
      .thickness = 44.0f,
  };

  glm_vec4_copy((vec4){0.5f, 0.5f, 0.5f, 1.0f}, grid_uniform.color);

  Mesh *grid = scene_new_mesh_fixed(&main_scene);
  gizmo_grid_create(grid, &(GizmoGridCreateDescriptor){
                              .uniform = grid_uniform,
                              .camera = main_scene.active_camera,
                              .viewport = &main_scene.viewport,
                              .device = renderer_device(&main_renderer),
                              .queue = renderer_queue(&main_renderer),
                          });
}

int main(int argc, const char *argv[]) {
  (void)argc, (void)argv; // unused

  printf("WASM INIT\n");

  // init renderer
  renderer_create(&main_renderer, &(RendererCreateDescriptor){
                                      .name = "canvas",
                                      .clock = &main_clock,
                                      .lock_mouse = true,
                                  });

  renderer_init(&main_renderer);

  // poll inputs
  input_listen();

  // set scene
  init_scene();

  // add gizmo camera
  GizmoCamera *new_cam = scene_add_camera(&main_scene,
                                          &(CameraCreateDescriptor){
                                              .speed = 20.0f,
                                              .clock = &main_clock,
                                              .mode = CameraMode_Fixed,
                                              .sensitivity = 0.2f,
                                              .wheel_sensitivity = 0.01f,
                                          },
                                          renderer_device(&main_renderer),
                                          renderer_queue(&main_renderer));

  gizmo_camera_lookat(new_cam, (vec3){10.0f, 2.0f, 0.0f},
                      (vec3){0.0f, 0.0f, 0.0f});

  // add transform gizmo
  GizmoTransformTranslate translate;
  gizmo_transform_translate_create(
      &translate, &(GizmoCreateDescriptor){
                      .camera = main_scene.active_camera,
                      .device = renderer_device(&main_renderer),
                      .queue = renderer_queue(&main_renderer),
                      .viewport = &main_scene.viewport,
                      .list = &main_scene.meshes,
                  });

  mesh_reference_list_transfert(&translate.meshes, &main_scene.layer.fixed);

  example_gltf(&main_scene, &main_renderer);

  add_grid();

  // Update Loop
  renderer_draw(&main_renderer, &main_scene, RendererDrawMode_Wireframe);

  // Quit
  renderer_close(&main_renderer);

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
