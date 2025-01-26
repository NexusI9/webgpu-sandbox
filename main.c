
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

#define SHADER_SOURCE(...) #__VA_ARGS__ // stringify (#) macros argument

static state_t state;

// callbacks
static int resize(int, const EmscriptenUiEvent *, void *);
static void draw();

void draw() {

  // update roation
  state.uniform.rot += 0.1f;
  state.uniform.rot = state.uniform.rot > 360.0f ? 0.0f : state.uniform.rot;
  // append update to queue
  wgpuQueueWriteBuffer(state.wgpu.queue, state.store.u_buffer, 0,
                       &state.uniform.rot, sizeof(state.uniform.rot));

  // create texture view
  WGPUTextureView back_buffer =
      wgpuSwapChainGetCurrentTextureView(state.wgpu.swapchain);
  WGPUCommandEncoder cmd_encoder =
      wgpuDeviceCreateCommandEncoder(state.wgpu.device, NULL);

  WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(
      cmd_encoder,
      &(WGPURenderPassDescriptor){
          .colorAttachmentCount = 1,
          .colorAttachments =
              &(WGPURenderPassColorAttachment){
                  .view = back_buffer,
                  .loadOp = WGPULoadOp_Clear,
                  .storeOp = WGPUStoreOp_Store,
                  .clearValue = (WGPUColor){0.2f, 0.2f, 0.6f, 1.0f},
                  .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
              },
      });

  // draw quads
  wgpuRenderPassEncoderSetPipeline(render_pass, state.wgpu.pipeline);
  wgpuRenderPassEncoderSetBindGroup(render_pass, 0, state.store.bind_group, 0,
                                    0);
  wgpuRenderPassEncoderSetVertexBuffer(render_pass, 0, state.store.v_buffer, 0,
                                       WGPU_WHOLE_SIZE);
  wgpuRenderPassEncoderSetIndexBuffer(render_pass, state.store.i_buffer,
                                      WGPUIndexFormat_Uint16, 0,
                                      WGPU_WHOLE_SIZE);

  // end render pass
  wgpuRenderPassEncoderEnd(render_pass);

  // create command buffer
  WGPUCommandBuffer cmd_buffer =
      wgpuCommandEncoderFinish(cmd_encoder, NULL); // after render pass ends
  // Submit command
  wgpuQueueSubmit(state.wgpu.queue, 1, &cmd_buffer);

  // clean up
  wgpuRenderPassEncoderRelease(render_pass);
  wgpuCommandEncoderRelease(cmd_encoder);
  wgpuCommandBufferRelease(cmd_buffer);
  wgpuTextureViewRelease(back_buffer);
}

void setup_triangle() {

  float const vertex_data[] = {
      -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,
      0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
      -0.5f, 0.5f,  1.0f, 1.0f, 1.0f,
  };

  uint16_t index_data[] = {
      0, 1, 2,
      0, 2, 3,
  };

  // set vertex and index buffer
  state.store.v_buffer = create_buffer(&state, vertex_data, sizeof(vertex_data),
                                       WGPUBufferUsage_Vertex);
  state.store.i_buffer = create_buffer(&state, vertex_data, sizeof(index_data),
                                       WGPUBufferUsage_Index);

  // setup uniform buffer
  state.store.u_buffer =
      create_buffer(&state, &state.uniform.rot, sizeof(state.uniform.rot),
                    WGPUBufferUsage_Uniform);

  state.store.bind_group = wgpuDeviceCreateBindGroup(
      state.wgpu.device, &(WGPUBindGroupDescriptor){
                             .layout = wgpuRenderPipelineGetBindGroupLayout(
                                 state.wgpu.pipeline, 0),
                             .entryCount = 1,
                             .entries =
                                 &(WGPUBindGroupEntry){
                                     .binding = 0,
                                     .offset = 0,
                                     .buffer = state.store.u_buffer,
                                     .size = sizeof(state.uniform.rot),
                                 },
                         });
}

void init_pipeline() {

  // Overall order
  // VERTEX ATTR____VERTEX BUFFER___BINDGROUP_____PIPELINE
  // TEXTURES______________________/             /
  // SHADER ____________________________________/

  // Loading shader
  void *shader_triangle_code;
  const char *shader_path = "./shader/default.wgsl";
  store_file(shader_triangle_code, shader_path);

  const char *shader_code = SHADER_SOURCE(
   struct VertexIn {
    @location(0) aPos : vec2<f32>, @location(1) aCol : vec3<f32>,
  };

  struct VertexOut {
    @location(0) vCol : vec3<f32>, @builtin(position) Position : vec4<f32>,
  };

  struct Rotation {
    @location(0) degs : f32,
  };

  @group(0) @binding(0) var<uniform> uRot : Rotation;

  // vertex shader
  @vertex fn vs_main(input : VertexIn) -> VertexOut {
    var rads : f32 = radians(uRot.degs);
    var cosA : f32 = cos(rads);
    var sinA : f32 = sin(rads);
    var rot : mat3x3<f32> = mat3x3<f32>(vec3<f32>(cosA, sinA, 0.0),
                                        vec3<f32>(-sinA, cosA, 0.0),
                                        vec3<f32>(0.0, 0.0, 1.0));
    var output : VertexOut;
    output.Position = vec4<f32>(vec3<f32>(input.aPos, 1.0), 1.0);
    output.vCol = input.aCol;
    return output;
  }

  // fragment shader
  @fragment fn fs_main(@location(0) vCol : vec3<f32>) -> @location(0)
      vec4<f32> {
    return vec4<f32>(vCol, 1.0);
  });

  // Compile shader
  WGPUShaderModule shader_triangle = create_shader(&state, shader_code, NULL);

  // layout vertex attributes
  WGPUVertexAttribute vert_attr[2] = {
      [0] = {.format = WGPUVertexFormat_Float32x2,
             .offset = 0,
             .shaderLocation = 0},
      [1] = {.format = WGPUVertexFormat_Float32x2,
             .offset = 2 * sizeof(float),
             .shaderLocation = 1}};

  // set buffer layout
  WGPUVertexBufferLayout vert_buffer_layout = {.arrayStride = 5 * sizeof(float),
                                               .attributeCount = 2,
                                               .attributes = vert_attr};

  // bind resources like textures, buffer and samplers
  // allow better gpu optimisation since layout is clearly defined
  WGPUBindGroupLayout bindgroup_layout = wgpuDeviceCreateBindGroupLayout(
      state.wgpu.device,
      &(WGPUBindGroupLayoutDescriptor){
          .entryCount = 1,
          .entries =
              &(WGPUBindGroupLayoutEntry){
                  .binding = 0,
                  .visibility = WGPUShaderStage_Vertex,
                  // buffer binding layout
                  .buffer = {.type = WGPUBufferBindingType_Uniform},
              },
      });

  WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(
      state.wgpu.device, &(WGPUPipelineLayoutDescriptor){
                             .bindGroupLayoutCount = 1,
                             .bindGroupLayouts = &bindgroup_layout,
                         });

  // create pipeline
  state.wgpu.pipeline = wgpuDeviceCreateRenderPipeline(
      state.wgpu.device,
      &(WGPURenderPipelineDescriptor){
          .layout = pipeline_layout,
          // vertex x shader link
          .vertex =
              {
                  .module = shader_triangle,
                  .entryPoint = "vs_main",
                  .bufferCount = 1,
                  .buffers = &vert_buffer_layout,
              },
          // draw settings
          .primitive =
              {
                  .frontFace = WGPUFrontFace_CCW,
                  .cullMode = WGPUCullMode_None,
                  .topology = WGPUPrimitiveTopology_TriangleList,
                  .stripIndexFormat = WGPUIndexFormat_Undefined,
              },
          .fragment =
              &(WGPUFragmentState){
                  .module = shader_triangle,
                  .entryPoint = "fs_main",
                  .targetCount = 1,
                  // Color targets
                  .targets =
                      &(WGPUColorTargetState){
                          .format = WGPUTextureFormat_BGRA8Unorm,
                          .writeMask = WGPUColorWriteMask_All,
                          .blend =
                              &(WGPUBlendState){
                                  .color =
                                      {
                                          .operation = WGPUBlendOperation_Add,
                                          .srcFactor = WGPUBlendFactor_One,
                                          .dstFactor = WGPUBlendFactor_One,
                                      },
                                  .alpha =
                                      {
                                          .operation = WGPUBlendOperation_Add,
                                          .srcFactor = WGPUBlendFactor_One,
                                          .dstFactor = WGPUBlendFactor_One,
                                      },
                              },
                      },
              },
          .multisample =
              {
                  .count = 1,
                  .mask = 0xFFFFFFFF,
                  .alphaToCoverageEnabled = false,
              },
          .depthStencil = NULL,
      });

  // clean up
  wgpuBindGroupLayoutRelease(bindgroup_layout);
  wgpuPipelineLayoutRelease(pipeline_layout);
  wgpuShaderModuleRelease(shader_triangle);
}

int main() {
  printf("WASM INIT\n");

  // setup state
  state.context.name = "canvas";
  state.wgpu.instance = wgpuCreateInstance(NULL);
  state.wgpu.device = emscripten_webgpu_get_device();
  state.wgpu.queue = wgpuDeviceGetQueue(state.wgpu.device);

  resize(0, NULL, NULL);
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, false,
                                 (em_ui_callback_func)resize);

  // Set pipeline
  init_pipeline();
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

#ifdef __cplusplus
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
}
