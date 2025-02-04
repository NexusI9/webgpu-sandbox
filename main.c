
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

#include "backend/generator.h"
#include "backend/state.h"

#include "runtime/camera.h"
#include "runtime/mesh.h"
#include "runtime/scene.h"
#include "runtime/shader.h"
#include "runtime/viewport.h"

static state_t state;
static scene main_scene;
static mesh tri_mesh;
static float rot = 0.0f;

// callbacks
static int resize(int, const EmscriptenUiEvent *, void *);
static void draw();
static void init_pipeline();
static void setup_triangle();
static void init_scene();

void draw() {

  // update rotation
  // state.uniform.rot += 0.1f;
  // state.uniform.rot = state.uniform.rot > 360.0f ? 0.0f : state.uniform.rot;

  // append update to queue
  // wgpuQueueWriteBuffer(state.wgpu.queue, state.store.u_buffer,
  // 0,&state.uniform.rot, sizeof(state.uniform.rot));

  // create texture view
  WGPUTextureView back_buffer =
      wgpuSwapChainGetCurrentTextureView(state.wgpu.swapchain);

  // create command encoder
  WGPUCommandEncoder cmd_encoder =
      wgpuDeviceCreateCommandEncoder(state.wgpu.device, NULL);

  // begin render pass
  WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(
      cmd_encoder,
      &(WGPURenderPassDescriptor){
          // color attachments
          .colorAttachmentCount = 1,
          .colorAttachments =
              &(WGPURenderPassColorAttachment){
                  .view = back_buffer,
                  .loadOp = WGPULoadOp_Clear,
                  .storeOp = WGPUStoreOp_Store,
                  .clearValue = (WGPUColor){0.2f, 0.2f, 0.3f, 1.0f},
                  .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED},
      });

  // draw mesh scene
  scene_draw(&main_scene, &render_pass);

  // end render pass
  wgpuRenderPassEncoderEnd(render_pass);

  // create command buffer
  WGPUCommandBuffer cmd_buffer =
      wgpuCommandEncoderFinish(cmd_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(state.wgpu.queue, 1, &cmd_buffer);

  // release all
  wgpuRenderPassEncoderRelease(render_pass);
  wgpuCommandEncoderRelease(cmd_encoder);
  wgpuCommandBufferRelease(cmd_buffer);
  wgpuTextureViewRelease(back_buffer);
}

void init_scene() {

  viewport vp = viewport_create(&(ViewportCreateDescriptor){
      .fov = 45.0f,
      .near_clip = 0.1f,
      .far_clip = 100.0f,
      .aspect = 1920.0f / 1080.0f,
  });
  camera cam = camera_create();

  main_scene = scene_create(cam, vp);
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

  shader triangle_shader =
      shader_create(&(ShaderCreateDescriptor){.path = "./shader/rotation.wgsl",
                                              .label = "triangle",
                                              .device = &state.wgpu.device,
                                              .queue = &state.wgpu.queue,
                                              .name = "triangle"});

  // bind the rotation uniform
  shader_add_uniform(&triangle_shader, &(ShaderCreateUniformDescriptor){
                                           .data = &rot,
                                           .size = sizeof(rot),
                                           .group_index = 0,
                                           .entry_count = 1,
                                           .entries =
                                               &(WGPUBindGroupEntry){
                                                   .binding = 0,
                                                   .offset = 0,
                                                   .size = sizeof(rot),
                                               },
                                       });

  printf("======> camera\n");
  // bind camera and viewport
  shader_bind_camera(&triangle_shader, &main_scene.camera, &main_scene.viewport,
                     1);

  tri_mesh = mesh_create(&(MeshCreateDescriptor){
      // wgpu object
      .wgpu =
          {
              .queue = &state.wgpu.queue,
              .device = &state.wgpu.device,
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

int main(int argc, const char *argv[]) {
  (void)argc, (void)argv; // unused

  printf("WASM INIT\n");

  // setup state
  state.context.name = "canvas";
  state.wgpu.instance = wgpuCreateInstance(NULL);
  state.wgpu.device = emscripten_webgpu_get_device();
  state.wgpu.queue = wgpuDeviceGetQueue(state.wgpu.device);

  resize(0, NULL, NULL);
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, false,
                                 (em_ui_callback_func)resize);

  init_scene();
  setup_triangle();

  // Update Loop
  emscripten_set_main_loop(draw, 0, 1);

  // Quit
  wgpuRenderPipelineRelease(state.wgpu.pipeline);
  wgpuSwapChainRelease(state.wgpu.swapchain);
  wgpuQueueRelease(state.wgpu.queue);
  wgpuDeviceRelease(state.wgpu.device);
  wgpuInstanceRelease(state.wgpu.instance);

  return 0;
}

int resize(int event_type, const EmscriptenUiEvent *ui_event, void *user_data) {
  double w, h;

  // retrieve canvas dimension
  emscripten_get_element_css_size(state.context.name, &w, &h);
  state.context.width = (int)w;
  state.context.height = (int)h;

  // set canvas size
  emscripten_set_element_css_size(state.context.name, state.context.width,
                                  state.context.height);

  // reset swap chain on resize
  if (state.wgpu.swapchain) {
    wgpuSwapChainRelease(state.wgpu.swapchain);
    state.wgpu.swapchain = NULL;
  }

  state.wgpu.swapchain = create_swapchain(&state);

  return 1;
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
