#include "shader.h"
#include "../backend/generator.h"
#include "../utils/file.h"
#include "webgpu/webgpu.h"
#include <stdio.h>

static void set_vertex_layout(shader *);
static void set_pipeline(shader *);

shader shader_create(const ShaderCreateDescriptor *sd) {
  shader shader;

  // store shader string in memory
  store_file(&shader.source, sd->path);

  // compile shader module intro GPU device
  shader.module = create_shader(sd->device, shader.source, sd->label);
  shader.device = sd->device;

  // define bind_group length
  shader.bind_groups.length = 0;

  // set vertex layout
  set_vertex_layout(&shader);

  // define pipeline
  set_pipeline(&shader);

  return shader;
}

WGPUBindGroup shader_add_bind_group(shader *shader,
                                    const ShaderBindGroupDescriptor *bd) {

  WGPUBindGroup bind_group = wgpuDeviceCreateBindGroup(
      *shader->device, &(WGPUBindGroupDescriptor){
                           .layout = wgpuRenderPipelineGetBindGroupLayout(
                               shader->pipeline, bd->group_index),
                           .entryCount = bd->entry_count,
                           // bind group entry
                           .entries = bd->entries,
                       });

  if (shader->bind_groups.length < SHADER_MAX_BIND_GROUP) {
    shader->bind_groups.items[shader->bind_groups.length++] = bind_group;
  } else {
    perror("Bind group list at full capacity");
  }

  return bind_group;
}

// Define vertex layout to be used in pipeline
void set_vertex_layout(shader *shader) {

  // set x,y
  shader->vertex.attribute[0] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x2,
      .offset = 0,
      .shaderLocation = 0,
  };

  // set r,g,b
  shader->vertex.attribute[1] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x3,
      .offset = 2 * sizeof(float),
      .shaderLocation = 1,
  };

  // define layout from attributes above
  shader->vertex.layout = (WGPUVertexBufferLayout){
      .arrayStride = 5 * sizeof(float),
      .attributeCount = 2,
      .attributes = shader->vertex.attribute,
  };
}

void set_pipeline(shader *shader) {

  WGPUBindGroupLayout bindgroup_layout = wgpuDeviceCreateBindGroupLayout(
      *shader->device,
      &(WGPUBindGroupLayoutDescriptor){
          .entryCount = 1,
          // bind group layout entry
          .entries =
              &(WGPUBindGroupLayoutEntry){
                  .binding = 0,
                  .visibility = WGPUShaderStage_Vertex,
                  // buffer binding layout
                  .buffer =
                      {
                          .type = WGPUBufferBindingType_Uniform,
                      }},
      });

  WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(
      *shader->device, &(WGPUPipelineLayoutDescriptor){
                           .bindGroupLayoutCount = 1,
                           .bindGroupLayouts = &bindgroup_layout,
                       });

  // create pipeline
  shader->pipeline = wgpuDeviceCreateRenderPipeline(
      *shader->device,
      &(WGPURenderPipelineDescriptor){
          .layout = pipeline_layout,
          .vertex =
              {
                  .module = shader->module,
                  .entryPoint = "vs_main",
                  .bufferCount = 1,
                  .buffers = &shader->vertex.layout,
              },
          .primitive =
              {
                  .frontFace = WGPUFrontFace_CCW,
                  .cullMode = WGPUCullMode_None,
                  .topology = WGPUPrimitiveTopology_TriangleList,
                  .stripIndexFormat = WGPUIndexFormat_Undefined,
              },
          .fragment =
              &(WGPUFragmentState){
                  .module = shader->module,
                  .entryPoint = "fs_main",
                  .targetCount = 1,
                  // color target state
                  .targets =
                      &(WGPUColorTargetState){
                          .format = WGPUTextureFormat_BGRA8Unorm,
                          .writeMask = WGPUColorWriteMask_All,
                          // blend state
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

  // clean layouts
  wgpuBindGroupLayoutRelease(bindgroup_layout);
  wgpuPipelineLayoutRelease(pipeline_layout);

  // clean shader module
  wgpuShaderModuleRelease(shader->module);
}

void shader_draw(const shader *shader, WGPURenderPassEncoder *render_pass,
                 const camera *camera, const viewport *viewport) {

  wgpuRenderPassEncoderSetPipeline(*render_pass, shader->pipeline);

  // update bind groupd (uniforms, projection/view matrix...)
  for (int i = 0; i < shader->bind_groups.length; i++) {
    wgpuRenderPassEncoderSetBindGroup(*render_pass, 0,
                                      shader->bind_groups.items[i], 0, 0);
  }
}
