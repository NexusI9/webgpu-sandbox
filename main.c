
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

#include "utils/file.h"

#include "backend/clock.h"
#include "backend/generator.h"
#include "backend/renderer.h"

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
      .mode = FLYING,
  });

  main_scene = scene_create(camera, viewport);
  // TODO: check if possible to set the mode in the descriptor

  // init camera position
  camera_look_at(&main_scene.camera, (vec3){0.0f, 0.0f, 10.0f},
                 (vec3){0.0f, 0.0f, 0.0f});
  // camera_translate(&main_scene.camera, (vec3){0.0f, 0.0f, 12.0f});
  camera_set_mode(&main_scene.camera, ORBIT);
}

void setup_triangle() {

  // create the vertex buffer (x, y, r, g, b) and index buffer
  const float vertex_data[] = {
      // Front face
      -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, // Bottom-left
      0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  // Bottom-right
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,   // Top-right
      -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,  // Top-left

      // Back face
      -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, // Bottom-left
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  // Bottom-right
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f,   // Top-right
      -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f,  // Top-left
  };

  uint16_t index_data[] = {// Front face
                           0, 1, 2, 0, 2, 3,

                           // Back face
                           5, 4, 7, 5, 7, 6,

                           // Left face
                           4, 0, 3, 4, 3, 7,

                           // Right face
                           1, 5, 6, 1, 6, 2,

                           // Top face
                           3, 2, 6, 3, 6, 7,

                           // Bottom face
                           4, 5, 1, 4, 1, 0};

  shader triangle_shader = shader_create(&(ShaderCreateDescriptor){
      .path = "./shader/rotation.wgsl",
      .label = "triangle",
      .name = "triangle",
      .device = &main_renderer.wgpu.device,
      .queue = &main_renderer.wgpu.queue,
  });

  // bind camera and viewport
  shader_bind_camera(&triangle_shader, &main_scene.camera, &main_scene.viewport,
                     0);

  tri_mesh = mesh_create(&(MeshCreateDescriptor){
      // wgpu object
      .wgpu =
          {
              .queue = &main_renderer.wgpu.queue,
              .device = &main_renderer.wgpu.device,
          },

      // vertex data
      .vertex =
          {
              .data = vertex_data,
              .length = sizeof(vertex_data) / sizeof(vertex_data[0]),
          },

      // index data
      .index =
          {
              .data = index_data,
              .length = sizeof(index_data) / sizeof(index_data[0]),
          },

      // shader
      .shader = triangle_shader,
  });

  // add triangle to scene
  scene_add_mesh(&main_scene, &tri_mesh);
}

void draw() {
  renderer_draw(&main_renderer, &main_scene);
  return;
}

int main(int argc, const char *argv[]) {
  (void)argc, (void)argv; // unused

  printf("WASM INIT\n");

  // init renderer
  main_renderer = renderer_create(
      &(RendererCreateDescriptor){.name = "canvas", .clock = &main_clock});

  renderer_init(&main_renderer);
  renderer_lock_mouse(&main_renderer);

  // poll inputs
  input_listen();

  // set scene
  init_scene();
  setup_triangle();

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
