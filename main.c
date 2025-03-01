
//  RESOURCES
//  https://github.com/seyhajin/webgpu-wasm-c
//  https://developer.chrome.com/docs/web-platform/webgpu/build-app?hl=en
//  https://stackoverflow.com/questions/23997312/how-do-i-read-a-user-specified-file-in-an-emscripten-compiled-library

#include <emscripten/emscripten.h>
#include <stdint.h>
#include <stdio.h>

// HEADERS
#include "emscripten/html5.h"
#include "emscripten/html5_webgpu.h"
#include <webgpu/webgpu.h>

#include "resources/debug/grid.h"
#include "utils/file.h"

#include "backend/clock.h"
#include "backend/generator.h"
#include "backend/renderer.h"

#include "resources/primitive/cube.h"
#include "resources/primitive/plane.h"
#include "runtime/camera.h"
#include "runtime/input.h"
#include "runtime/mesh.h"
#include "runtime/scene.h"
#include "runtime/shader.h"
#include "runtime/viewport.h"

static scene main_scene;
static mesh tri_mesh;
static float rot = 0.0f;
static renderer main_renderer;
static cclock main_clock;

// callbacksd
static void init_pipeline();
static void setup_triangle();
static void init_scene();

void init_scene() {

  // set viewport
  viewport viewport = viewport_create(&(ViewportCreateDescriptor){
      .fov = 34.0f,
      .near_clip = 0.1f,
      .far_clip = 100.0f,
      .aspect = 1920.0f / 1080.0f,
  });

  // set camera
  camera camera = camera_create(&(CameraCreateDescriptor){
      .speed = 20.0f,
      .clock = &main_clock,
      .mode = ORBIT,
  });

  main_scene = scene_create(camera, viewport);
  // TODO: check if possible to set the mode in the descriptor

  // init camera position
  camera_look_at(&main_scene.camera, (vec3){0.0f, 0.0f, 10.0f},
                 (vec3){0.0f, 0.0f, 0.0f});
}

void setup_grid() {

  GridUniform grid_uniform = {
      .size = 20.0f,
      .cell_size = 5.0f,
      .thickness = 0.3f,
  };

  glm_vec4_copy((vec4){1.0f, 1.0f, 0.0f, 1.0f}, grid_uniform.color);

  mesh grid = grid_create_mesh(&(GridCreateDescriptor){
      .uniform = grid_uniform,
      .camera = &main_scene.camera,
      .viewport = &main_scene.viewport,
      .device = &main_renderer.wgpu.device,
      .queue = &main_renderer.wgpu.queue,
  });

  // add triangle to scene
  scene_add_mesh(&main_scene, grid);
}

void draw() {
  renderer_draw(&main_renderer, &main_scene);
  return;
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
  setup_grid();

  // Update Loop
  renderer_set_draw(draw);

  // Quit
  renderer_end_frame(&main_renderer);

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
