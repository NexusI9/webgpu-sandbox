#include "pipeline.h"
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
      .color = {.operation = WGPUBlendOperation_Add,
                .srcFactor = WGPUBlendFactor_SrcAlpha,
                .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha},
      .alpha = {.operation = WGPUBlendOperation_Add,
                .srcFactor = WGPUBlendFactor_One,
                .dstFactor = WGPUBlendFactor_Zero},
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
   Define custom vertex state for pipeline prior building it
 */
void pipeline_set_vertex(Pipeline *pipeline, const WGPUVertexState state) {
  pipeline->vertex_state = state;
}

/**
   Define custom fragment state for pipeline prior building it
 */
void pipeline_set_fragment(Pipeline *pipeline,
                           const PipelineFragmentDescriptor *state) {

  // cache attributes
  pipeline->color_state = state->color_state;
  pipeline->blend_state = state->blend_state;

  // set base attributes
  pipeline->fragment_state = (WGPUFragmentState){
      .module = state->fragment_state.module,
      .entryPoint = state->fragment_state.entryPoint,
      .targetCount = state->fragment_state.targetCount,
  };

  if (pipeline->color_state.format != 0) {

    // set color base (dirty)
    pipeline->color_state = (WGPUColorTargetState){
        .format = state->color_state.format,
        .writeMask = state->color_state.writeMask,
    };

    // 1. plug blend -> color state
    if (pipeline->blend_state.alpha.operation != 0)
      pipeline->color_state.blend = &pipeline->blend_state;

    // 2. plug color state -> pipeline
    pipeline->fragment_state.targets = &pipeline->color_state;
  }
}

/**
   Define custom primitive state for pipeline prior building it
 */
void pipeline_set_primitive(Pipeline *pipeline,
                            const WGPUPrimitiveState state) {
  pipeline->primitive_state = state;
}

/**
   Define custom stencil state for pipeline prior building it
 */
void pipeline_set_stencil(Pipeline *pipeline,
                          const WGPUDepthStencilState state) {
  pipeline->stencil_state = state;
}

/**
   Release pipeline if exists and create i new one
 */
void pipeline_build(Pipeline *pipeline, const WGPUPipelineLayout *layout) {

  printf("pipeline layout: %p\n", pipeline->layout);
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
              .count = 1,
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
