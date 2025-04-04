#include "pipeline.h"
#include "emscripten/emscripten.h"
#include "string.h"
#include "webgpu/webgpu.h"
#include <stdbool.h>
#include <stdio.h>

/**
  Initialize the default pipeline with a preset descriptor
 */
void pipeline_create(pipeline *pipeline, const PipelineCreateDescriptor *desc) {

  // Define core data
  pipeline->vertex_layout = desc->vertex_layout;
  pipeline->module = desc->module;
  pipeline->device = desc->device;
  pipeline->handle = NULL;

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

  // Fragment State
  pipeline->fragment_state = (WGPUFragmentState){
      .module = *pipeline->module,
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
                              .srcFactor = WGPUBlendFactor_SrcAlpha,
                              .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
                          },
                      .alpha =
                          {
                              .operation = WGPUBlendOperation_Add,
                              .srcFactor = WGPUBlendFactor_One,
                              .dstFactor = WGPUBlendFactor_Zero,
                          },
                  },
          },
  };

  // Stencil State
  pipeline->stencil_state = (WGPUDepthStencilState){
      .format = WGPUTextureFormat_Depth24Plus,
      .depthWriteEnabled = true,
      .depthCompare = WGPUCompareFunction_Less,
  };

  // define pipeline
  pipeline->descriptor =
      (WGPURenderPipelineDescriptor){.label = "Shader",
                                     .primitive = pipeline->primitive_state,
                                     .vertex = pipeline->vertex_state,
                                     .fragment = &pipeline->fragment_state,
                                     .multisample =
                                         {
                                             .count = 1,
                                             .mask = 0xFFFFFFFF,
                                             .alphaToCoverageEnabled = false,
                                         },
                                     .depthStencil = &pipeline->stencil_state

      };

  printf("create vertex layout format: %u\n",
         pipeline->vertex_layout->attributes->format);
}

/**
   Define custom vertex state for pipeline prior building it
 */
void pipeline_set_vertex(pipeline *pipeline, WGPUVertexState state) {
  pipeline->vertex_state = state;
}

/**
   Define custom fragment state for pipeline prior building it
 */
void pipeline_set_fragment(pipeline *pipeline, WGPUFragmentState state) {
  pipeline->fragment_state = state;
}

/**
   Define custom primitive state for pipeline prior building it
 */
void pipeline_set_primitive(pipeline *pipeline, WGPUPrimitiveState state) {
  pipeline->primitive_state = state;
}

/**
   Define custom stencil state for pipeline prior building it
 */
void pipeline_set_stencil(pipeline *pipeline, WGPUDepthStencilState state) {
  pipeline->stencil_state = state;
}

/**
   Release pipeline if exists and create i new one
 */
void pipeline_build(pipeline *pipeline, WGPUPipelineLayout *layout) {
  printf("Build vertex layout format: %u\n",
         pipeline->vertex_layout->attributes->format);

  // apply layout to pipeline descriptor
  pipeline->descriptor.layout = *layout;

  if (pipeline->handle)
    pipeline_clear(pipeline);

  // update bind group layout
  pipeline->layout = *layout;

  pipeline->handle =
      wgpuDeviceCreateRenderPipeline(*pipeline->device, &pipeline->descriptor);
}

/**
   Release pipeline and set back the handle to null
 */
void pipeline_clear(pipeline *pipeline) {
  wgpuRenderPipelineRelease(pipeline->handle);
  pipeline->handle = NULL;

  // wgpuPipelineLayoutRelease(pipeline->layout);
  pipeline->layout = NULL;

}
