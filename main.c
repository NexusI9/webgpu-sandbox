//  RESOURCES
//  https://github.com/seyhajin/webgpu-wasm-c
//  https://developer.chrome.com/docs/web-platform/webgpu/build-app?hl=en
//  https://stackoverflow.com/questions/23997312/how-do-i-read-a-user-specified-file-in-an-emscripten-compiled-library

#include "backend/renderer.h"
#include "resources/example/example.h"
#include <emscripten/emscripten.h>

// runtime
#include "runtime/prefab/environment/skybox.h"
#include "runtime/scene/core.h"

static Scene main_scene;
static Renderer main_renderer;
static cclock main_clock;

// callback
static void init_scene();

void init_scene() {

  scene_create(&main_scene,
               &(SceneCreateDescriptor){
                   .clock = &main_clock,
                   .viewport =
                       &(ViewportCreateDescriptor){
                           .fov = 32.0f,
                           .near_clip = 0.1f,
                           .far_clip = 100.0f,
                           .aspect = 16.0f / 9.0f,
                           .width = renderer_width(&main_renderer),
                           .height = renderer_height(&main_renderer),
                       },
                   .device = renderer_device(&main_renderer),
                   .queue = renderer_queue(&main_renderer),
               });

  /*

  =============        CAMERA       ==============

 */
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

  /*

    =============        LIGHTS       ==============

   */

  // dir light
  static vec3 LIGHT_POSITION = {0.0f, 4.0f, 6.0f};
  static vec3 LIGHT_TARGET = {0.0f, 0.0f, 0.0f};

  // point light
  static vec3 POINT_LIGHT = {0.0f, 2.4f, 2.3f};

  // sun light
  static vec3 SUN_LIGHT = {-2.0f, 2.0f, 2.0f};

  scene_add_sun_light(
      &main_scene, &(SunLightDescriptor){
                       .position = {SUN_LIGHT[0], SUN_LIGHT[1], SUN_LIGHT[2]},
                       .color = {1.0f, 1.0f, 1.0f},
                       .intensity = 1.0f,
                       .size = 10.0f,
                   });

  // set light
  scene_add_point_light(&main_scene, &(PointLightDescriptor){
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
                                     });

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

  scene_add_ambient_light(&main_scene, &(AmbientLightDescriptor){
                                           .color = {1.0f, 1.0f, 1.0f},
                                           .intensity = 0.2f,
                                           .position = {2.0f, 4.0f, -5.0f},
                                       });

  /*

  =============        SKYBOX       ==============

   */

  prefab_skybox_create(
      &(PrefabCreateDescriptor){
          .device = renderer_device(&main_renderer),
          .queue = renderer_queue(&main_renderer),
          .scene = &main_scene,
      },
      &(PrefabSkyboxCreateDescriptor){
          .blur = 0.0f,
          .resolution = 1024,
          .path =
              {
                  .right = "./resources/assets/texture/skybox/lake/right.png",
                  .left = "./resources/assets/texture/skybox/lake/left.png",
                  .top = "./resources/assets/texture/skybox/lake/top.png",
                  .bottom = "./resources/assets/texture/skybox/lake/bottom.png",
                  .front = "./resources/assets/texture/skybox/lake/front.png",
                  .back = "./resources/assets/texture/skybox/lake/back.png",
              },
      });

  /* prefab_skybox_gradient_create(
       &(PrefabCreateDescriptor){
           .device = renderer_device(&main_renderer),
           .queue = renderer_queue(&main_renderer),
           .scene = &main_scene,
       },
       &(PrefabSkyboxGradientCreateDescriptor){
           .resolution = 32,
           .stops =
               {
                   .length = 2,
                   .capacity = 2,
                   .entries =
                       (TextureGradientStop[]){
                           {
                               .color = (uint8_t[]){240, 245, 255, 255},
                               .position = 1.0f,
                           },
                           {
                               .color = (uint8_t[]){51, 153, 255, 255},
                               .position = 0.0f,
                           },
                       },
               },
       });*/
}

void on_camera_raycast(CameraRaycastCallback *cast_data, void *user_data) {}

int main(int argc, const char *argv[]) {
  (void)argc, (void)argv; // unused

  printf("WASM INIT\n");

  // init renderer
  renderer_create(&main_renderer,
                  &(RendererCreateDescriptor){
                      .name = "canvas",
                      .clock = &main_clock,
                      .multisampling_count = PipelineMultisampleCount_4x,
                      .background = (WGPUColor){0.1f, 0.1f, 0.1f, 1.0f},
                      .dpi = 1.0,
                  });

  renderer_init(&main_renderer);

  // set scene
  init_scene();

  // raycast camera
  camera_raycast_mouse_hover(
      main_scene.active_camera,
      &(CameraRaycastDescriptor){
          .mesh_lists = (MeshRefList *[]){&main_scene.pipelines.fixed},
          .length = 1,
          .viewport = &main_scene.viewport,
          .callback = on_camera_raycast,
          .data = NULL,
      });

  // add gizmo camera
  GizmoCamera *new_cam =
      scene_add_camera(&main_scene, &(CameraCreateDescriptor){
                                        .speed = 20.0f,
                                        .clock = &main_clock,
                                        .mode = CameraMode_Fixed,
                                        .sensitivity = {0},
                                    });

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

  mesh_reference_list_transfert(&translate.meshes, &main_scene.pipelines.fixed);

  example_gltf(&main_scene, &main_renderer);

  // Update Loop
  renderer_draw(&main_renderer, &main_scene, RendererDrawMode_Texture);

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
