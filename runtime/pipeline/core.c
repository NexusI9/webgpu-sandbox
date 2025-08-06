#include "core.h"
#include "../backend/buffer.h"
#include "../utils/file.h"
#include "../utils/system.h"
#include "emscripten/emscripten.h"
#include "string.h"
#include "webgpu/webgpu.h"
#include <stdbool.h>
#include <stdio.h>

static inline void pipeline_set_vertex_layout(Pipeline *);

/**
  Initialize the default pipeline with a preset descriptor
 */
void pipeline_create(Pipeline *pipeline, const PipelineCreateDescriptor *desc) {

  // Define core data
  pipeline->device = desc->device;
  pipeline->sampling = PipelineMultisampleCount_1x;
  pipeline->handle = NULL;
  pipeline->label = desc->label;
  pipeline_set_vertex_layout(pipeline);

  char *source; // shader source code

  // store shader string in memory
  store_file(&source, desc->path);

  // compile shader module intro GPU device
  buffer_create_shader(&pipeline->module, pipeline->device, source,
                       pipeline->label);

  /*
    DEFINE PIPELINE CACHED ATTRIBUTES
    Define default layout, the default layout correspond to the texture shader
    layout (including fragment + vertex + depth stencil)
   */

  // Vertex State
  pipeline->vertex_state = (WGPUVertexState){
      .module = pipeline->module,
      .entryPoint = "vs_main",
      .bufferCount = 1,
      .buffers = &pipeline->vertex_layout.buffer,
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
      .module = pipeline->module,
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
   Define standard vertex layout to be used in pipeline
   1. Position (vec3)
   2. Normals (vec3)
   3. Color (vec3)
   4. Texture Coordinate (vec2)
 */
void pipeline_set_vertex_layout(Pipeline *pipeline) {

  // set x,y,z
  pipeline->vertex_layout.attribute[0] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x3,
      .offset = 0,
      .shaderLocation = 0,
  };

  // set normals
  pipeline->vertex_layout.attribute[1] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x3,
      .offset = 3 * sizeof(float),
      .shaderLocation = 1,
  };

  // set r,g,b
  pipeline->vertex_layout.attribute[2] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x3,
      .offset = 6 * sizeof(float),
      .shaderLocation = 2,
  };

  // set u,v
  pipeline->vertex_layout.attribute[3] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x2,
      .offset = 9 * sizeof(float),
      .shaderLocation = 3,
  };

  // define layout from attributes above
  pipeline->vertex_layout.buffer = (WGPUVertexBufferLayout){
      .arrayStride = VERTEX_STRIDE * sizeof(float),
      .attributeCount = 4,
      .attributes = pipeline->vertex_layout.attribute,
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
      .label = pipeline->label,
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
      wgpuDeviceCreateRenderPipeline(pipeline->device, &pipeline->descriptor);
}

/**
   Release pipeline and set back the handle to null
 */
void pipeline_destroy(Pipeline *pipeline) {

  // clearing module
  wgpuShaderModuleRelease(pipeline->module);

  wgpuShaderModuleRelease(pipeline->module);
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
