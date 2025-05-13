//  RESOURCES
//  https://github.com/seyhajin/webgpu-wasm-c
//  https://developer.chrome.com/docs/web-platform/webgpu/build-app?hl=en
//  https://stackoverflow.com/questions/23997312/how-do-i-read-a-user-specified-file-in-an-emscripten-compiled-library

#include <emscripten/emscripten.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

// HEADERS
#include "emscripten/html5.h"
#include "emscripten/html5_webgpu.h"
#include <webgpu/webgpu.h>

#include "resources/debug/grid.h"
#include "resources/debug/line.h"
#include "runtime/light.h"
#include "utils/file.h"

#include "backend/buffer.h"
#include "backend/clock.h"
#include "backend/renderer.h"

#include "resources/loader/loader.gltf.h"
#include "resources/primitive/cube.h"
#include "resources/primitive/plane.h"
#include "runtime/camera.h"
#include "runtime/input.h"
#include "runtime/material.h"
#include "runtime/mesh.h"
#include "runtime/scene.h"
#include "runtime/shader.h"
#include "runtime/viewport.h"
#include "utils/system.h"

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
  Viewport viewport = viewport_create(&(ViewportCreateDescriptor){
      .fov = 32.0f,
      .near_clip = 0.1f,
      .far_clip = 100.0f,
      .aspect = 16.0f / 9.0f,
  });

  // set camera
  Camera camera = camera_create(&(CameraCreateDescriptor){
      .speed = 20.0f,
      .clock = &main_clock,
      .mode = FLYING,
      .sensitivity = 0.2f,
      .wheel_sensitivity = 0.01f,
  });

  main_scene = scene_create(camera, viewport);

  // init camera position
  camera_look_at(&main_scene.camera,
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

  // set light
  /*scene_add_point_light(&main_scene, &(PointLightDescriptor){
                                         .color = {1.0f, 1.0f, 1.0f},
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
                                             });*/

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

  scene_add_sun_light(
      &main_scene, &(SunLightDescriptor){
                       .position = {SUN_LIGHT[0], SUN_LIGHT[1], SUN_LIGHT[2]},
                       .color = {1.0f, 1.0f, 1.0f},
                       .intensity = 4.0f,
                       .size = 10.0f,
                   });

  scene_add_ambient_light(&main_scene, &(AmbientLightDescriptor){
                                           .color = {1.0f, 1.0f, 1.0f},
                                           .intensity = 0.2f,
                                       });
}

void add_gizmo(){

    Mesh* gizmo = scene_new_mesh_fixed(&main_scene);
    
   
    
}

void add_cube(vec3 position) {

  Primitive cube_prim = primitive_cube();
  Mesh *cube = scene_new_mesh_unlit(&main_scene);
  mesh_create_primitive(cube, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = cube_prim,
                                  .name = "cube",
                                  .device = &main_renderer.wgpu.device,
                                  .queue = &main_renderer.wgpu.queue,
                              });

  mesh_set_shader(cube,
                  &(ShaderCreateDescriptor){
                      .path = "./runtime/assets/shader/shader.default.wgsl",
                      .label = "cube",
                      .name = "cube",
                      .device = &main_renderer.wgpu.device,
                      .queue = &main_renderer.wgpu.queue,
                  });

  mesh_position(cube, position);

  material_texture_bind_views(cube, &main_scene.camera, &main_scene.viewport,
                              0);
}

void add_grid() {

  GridUniform grid_uniform = {
      .size = 100.0f,
      .cell_size = 100.0f,
      .thickness = 44.0f,
  };

  glm_vec4_copy((vec4){0.5f, 0.5f, 0.5f, 1.0f}, grid_uniform.color);

  Mesh *grid = scene_new_mesh_fixed(&main_scene);
  grid_create(grid, &(GridCreateDescriptor){
                        .uniform = grid_uniform,
                        .camera = &main_scene.camera,
                        .viewport = &main_scene.viewport,
                        .device = &main_renderer.wgpu.device,
                        .queue = &main_renderer.wgpu.queue,
                    });
}

void add_line() {

  Mesh *line = scene_new_mesh_unlit(&main_scene);
  line_create(line, &(LineCreateDescriptor){
                        .device = &main_renderer.wgpu.device,
                        .queue = &main_renderer.wgpu.queue,
                        .name = "line mesh",
                    });

  line_add_point((vec3){-2.0f, -4.0f, -2.0f}, (vec3){2.0f, 4.0f, 2.0f},
                 (vec3){1.0f, 1.0f, 1.0f}, &line->vertex.base.attribute,
                 &line->vertex.base.index);

  line_add_point((vec3){3.0f, -2.0f, -2.0f}, (vec3){-3.0f, 7.0f, 3.0f},
                 (vec3){0.0f, 1.0f, 0.0f}, &line->vertex.base.attribute,
                 &line->vertex.base.index);

  material_texture_bind_views(line, &main_scene.camera, &main_scene.viewport,
                              0);
}

void import_cube() {

  loader_gltf_load(&(GLTFLoadDescriptor){
      .scene = &main_scene,
      .path = "./resources/assets/gltf/cube.gltf",
      .device = &main_renderer.wgpu.device,
      .queue = &main_renderer.wgpu.queue,
      .cgltf_options = &(cgltf_options){0},
  });
}

int main(int argc, const char *argv[]) {
  (void)argc, (void)argv; // unused

  printf("WASM INIT\n");

  // init renderer
  main_renderer = renderer_create(&(RendererCreateDescriptor){
      .name = "canvas",
      .clock = &main_clock,
      .lock_mouse = true,
  });

  renderer_init(&main_renderer);

  // poll inputs
  input_listen();

  // set scene
  init_scene();

  /*mesh child_cube;
  add_cube(&child_cube, (vec3){3.0f, 2.0f, 1.0f});
  mesh child_cube_A;
  add_cube(&child_cube_A, (vec3){-4.0f, -2.0f, -1.0f});
  add_cube(&child_cube_B, (vec3){-3.0f, -9.0f, 1.0f});
  // mesh_add_child(&child_cube, &parent_cube);
  // mesh_add_child(&child_cube_A, &parent_cube);
  // mesh_add_child(&child_cube_B, &parent_cube);
  */
  //  add_cube((vec3){1.0f, 0.0f, 0.0f});

  /* add_cube((vec3){
      POINT_LIGHT[0],
      POINT_LIGHT[1],
      POINT_LIGHT[2],
  });
  */
  import_cube();

  add_gizmo();
  // add_line();
  add_grid();

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
