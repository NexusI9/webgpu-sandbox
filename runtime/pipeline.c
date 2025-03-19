#include "pipeline.h"
#include "webgpu/webgpu.h"
#include <stdbool.h>

void pipeline_create_default(pipeline *pipeline,
                             const PipelineCreateDescriptor *desc) {

  pipeline->vertex_layout = desc->vertex_layout;
  pipeline->layout = desc->layout;
  pipeline->module = desc->module;
  pipeline->device = desc->device;
  pipeline->descriptor = (WGPURenderPipelineDescriptor){
      .layout = pipeline->layout,
      .label = "Shader",
      .vertex =
          {
              .module = *pipeline->module,
              .entryPoint = "vs_main",
              .bufferCount = 1,
              .buffers = pipeline->vertex_layout,
          },
      .primitive =
          {
              .frontFace = WGPUFrontFace_CCW,
              .cullMode = WGPUCullMode_Back,
              .topology = WGPUPrimitiveTopology_TriangleList,
              .stripIndexFormat = WGPUIndexFormat_Undefined,
          },
      .fragment =
          &(WGPUFragmentState){
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
                                      .dstFactor =
                                          WGPUBlendFactor_OneMinusSrcAlpha,
                                  },
                              .alpha =
                                  {
                                      .operation = WGPUBlendOperation_Add,
                                      .srcFactor = WGPUBlendFactor_One,
                                      .dstFactor = WGPUBlendFactor_Zero,
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
      .depthStencil =
          &(WGPUDepthStencilState){
              .format = WGPUTextureFormat_Depth24Plus,
              .depthWriteEnabled = true,
              .depthCompare = WGPUCompareFunction_Less,
          },

  };

  pipeline->handle =
      wgpuDeviceCreateRenderPipeline(*pipeline->device, &pipeline->descriptor);
}

void pipeline_set_cullmode(pipeline *pipeline, WGPUCullMode cullmode) {}
