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

  pipeline->vertex_layout = desc->vertex_layout;
  pipeline->module = desc->module;
  pipeline->device = desc->device;
  pipeline->custom_attributes = NULL;
  pipeline->handle = NULL;

  printf("create vertex layout format: %u\n",
         pipeline->vertex_layout->attributes->format);
}

/**
   Release pipeline if exists and create i new one
 */
void pipeline_build(pipeline *pipeline, WGPUPipelineLayout *layout) {
  printf("Build vertex layout format: %u\n",
         pipeline->vertex_layout->attributes->format);

  if (pipeline->handle)
    pipeline_clear(pipeline);

  // custom attributes
  WGPUCullMode cull_mode = pipeline->custom_attributes != NULL
                               ? pipeline->custom_attributes->cullMode
                               : PIPELINE_DEFAULT_CULLMODE;

  // update bind group layout
  pipeline->layout = *layout;
  pipeline->descriptor = (WGPURenderPipelineDescriptor){
      .layout = *layout,
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
              .cullMode = cull_mode,
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

/**
   Release pipeline and set back the handle to null
 */
void pipeline_clear(pipeline *pipeline) {
  wgpuRenderPipelineRelease(pipeline->handle);
  pipeline->handle = NULL;

  // wgpuPipelineLayoutRelease(pipeline->layout);
  pipeline->layout = NULL;

  // reset custom attributes
  if (pipeline->custom_attributes != NULL) {
    free(pipeline->custom_attributes);
    pipeline->custom_attributes = NULL;
  }
}

/**
   Initialize the custom attributes that will overide the default during the
   pipeline build
 */
void pipeline_set_custom(pipeline *pipeline,
                         PipelineCustomAttributes *attributes) {

  if (pipeline->custom_attributes == NULL)
    pipeline->custom_attributes =
        (PipelineCustomAttributes *)malloc(sizeof(PipelineCustomAttributes));

  *pipeline->custom_attributes = (PipelineCustomAttributes){
      .cullMode = attributes->cullMode,
  };
}
