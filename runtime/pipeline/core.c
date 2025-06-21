#include "core.h"
#include "emscripten/emscripten.h"
#include "string.h"
#include "webgpu/webgpu.h"
#include <stdbool.h>
#include <stdio.h>

/**
  Initialize the default pipeline with a preset descriptor
 */
void pipeline_create(Pipeline *pipeline, const PipelineCreateDescriptor *desc) {

  // Define core data
  pipeline->vertex_layout = desc->vertex_layout;
  pipeline->module = desc->module;
  pipeline->device = desc->device;
  pipeline->sampling = 1;
  pipeline->handle = NULL;

  /*
    DEFINE PIPELINE CACHED ATTRIBUTES
    Define default layout, the default layout correspond to the texture shader
    layout (including fragment + vertex + depth stencil)
   */

  // Vertex State
  pipeline->vertex_state = (WGPUVertexState){
      .module = *pipeline->module,
      .entryPoint = "vs_main",
      .bufferCount = 1,
      .buffers = pipeline->vertex_layout,
  };

  // Primitive State
  pipeline->primitive_state = (WGPUPrimitiveState){
      .frontFace = WGPUFrontFace_CCW,
      .cullMode = WGPUCullMode_Back,
      .topology = WGPUPrimitiveTopology_TriangleList,
      .stripIndexFormat = WGPUIndexFormat_Undefined,
  };

  // Blend State
  pipeline->blend_state = (WGPUBlendState){
      .color =
          {
              .operation = WGPUBlendOperation_Add,
              .srcFactor = WGPUBlendFactor_SrcAlpha,
              .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
          },
      .alpha =
          {
              .operation = WGPUBlendOperation_Add,
              .srcFactor = WGPUBlendFactor_One,
              .dstFactor = WGPUBlendFactor_Zero,
          },
  };

  // Color State
  pipeline->color_state = (WGPUColorTargetState){
      .format = WGPUTextureFormat_BGRA8Unorm,
      .writeMask = WGPUColorWriteMask_All,
      .blend = &pipeline->blend_state,
  };

  // Fragment State
  pipeline->fragment_state = (WGPUFragmentState){
      .module = *pipeline->module,
      .entryPoint = "fs_main",
      .targetCount = 1,
      .targets = &pipeline->color_state,

  };

  // Stencil State
  pipeline->stencil_state = (WGPUDepthStencilState){
      .format = WGPUTextureFormat_Depth24Plus,
      .depthWriteEnabled = true,
      .depthCompare = WGPUCompareFunction_Less,
  };
}

/**
   Release pipeline if exists and create i new one
 */
void pipeline_build(Pipeline *pipeline, const WGPUPipelineLayout *layout) {

  // update bind group layout
  pipeline->layout = *layout;

  // transfert cached states to pipeline
  pipeline->descriptor = (WGPURenderPipelineDescriptor){
      .layout = pipeline->layout,
      .label = "Shader",
      .vertex = pipeline->vertex_state,
      .primitive = pipeline->primitive_state,
      .multisample =
          {
              .count = pipeline->sampling,
              .mask = 0xFFFFFFFF,
              .alphaToCoverageEnabled = false,
          },
  };

  // add optional fragment or vertex
  if (pipeline->fragment_state.module != 0)
    pipeline->descriptor.fragment = &pipeline->fragment_state;

  if (pipeline->stencil_state.format != 0)
    pipeline->descriptor.depthStencil = &pipeline->stencil_state;

  if (pipeline->handle)
    pipeline_destroy(pipeline);

  pipeline->handle =
      wgpuDeviceCreateRenderPipeline(*pipeline->device, &pipeline->descriptor);
}

/**
   Release pipeline and set back the handle to null
 */
void pipeline_destroy(Pipeline *pipeline) {
  wgpuRenderPipelineRelease(pipeline->handle);
  pipeline->handle = NULL;

  // DO NOT uncomment, it causes crashes,
  // probably cause the layout is still in use
  // wgpuPipelineLayoutRelease(pipeline->layout);
  pipeline->layout = NULL;
}

void pipeline_set_sampling(Pipeline *pipeline,
                           PipelineMultisampleCount sampling) {
  pipeline->sampling = sampling;
}
