#include "render.h"

#include <stdbool.h>

#include "backend/buffer.h"
#include "backend/context.h"
#include "backend/registry.h"
#include "runtime/geometry/vertex/core.h"
#include "runtime/texture/core.h"
#include "utils/file.h"
#include "webgpu/webgpu.h"

static inline void render_pipeline_set_vertex_layout(RenderPipeline *);

/**
  Initialize the default pipeline with a preset descriptor
 */
void render_pipeline_create(RenderPipeline *pipeline,
                            const RenderPipelineCreateDescriptor *desc) {

  // Define core data
  pipeline->id = reg_register(pipeline, RegEntryType_RenderPipeline);
  pipeline->handle = NULL;
  pipeline->label = desc->label;
  pipeline->shader_pso = desc->pso;
  pipeline->bindings = pipeline->shader_pso->bindings;
  render_pipeline_set_vertex_layout(pipeline);

  char *source; // shader source code

  // store shader string in memory
  store_file(&source, desc->path);

  // compile shader module intro GPU device
  buffer_create_shader(&pipeline->module, source, pipeline->label);

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
              .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
          },
  };

  // Color State
  pipeline->color_state = (WGPUColorTargetState){
      .format = TEXTURE_FORMAT_ONSCREEN,
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
      .format = TEXTURE_FORMAT_DEPTH_STENCIL,
      .depthWriteEnabled = true,
      .depthCompare = WGPUCompareFunction_Less,
  };

  pipeline->multisample_state = (WGPUMultisampleState){
      .count = PipelineMultisampleCount_1x,
      .mask = 0xFFFFFFFF,
      .alphaToCoverageEnabled = false,
  };
}

/**
   Define standard vertex layout to be used in pipeline
   1. Position (vec3)
   2. Normals (vec3)
   3. Color (vec3)
   4. Texture Coordinate (vec2)
 */
void render_pipeline_set_vertex_layout(RenderPipeline *pipeline) {

  // set x,y,z
  pipeline->vertex_layout.attribute[0] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x3,
      .offset = VertexAttributeOffset_Position,
      .shaderLocation = 0,
  };

  // set normals
  pipeline->vertex_layout.attribute[1] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x3,
      .offset = VertexAttributeOffset_Normal * sizeof(float),
      .shaderLocation = 1,
  };

  // set tangent
  pipeline->vertex_layout.attribute[2] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x4,
      .offset = VertexAttributeOffset_Tangent * sizeof(float),
      .shaderLocation = 2,
  };

  // set r,g,b
  pipeline->vertex_layout.attribute[3] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x3,
      .offset = VertexAttributeOffset_Color * sizeof(float),
      .shaderLocation = 3,
  };

  // set u,v
  pipeline->vertex_layout.attribute[4] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x2,
      .offset = VertexAttributeOffset_Uv * sizeof(float),
      .shaderLocation = 4,
  };

  // define layout from attributes above
  pipeline->vertex_layout.buffer = (WGPUVertexBufferLayout){
      .arrayStride = VERTEX_STRIDE * sizeof(float),
      .attributeCount = VERTEX_ATTRIBUTE_COUNT,
      .attributes = pipeline->vertex_layout.attribute,
  };
}

/**
   Release pipeline if exists and create i new one
 */
void render_pipeline_build(RenderPipeline *pipeline,
                           const WGPUPipelineLayout *layout) {

  // update bind group layout
  pipeline->layout = *layout;

  // transfert cached states to pipeline
  pipeline->descriptor = (WGPURenderPipelineDescriptor){
      .layout = pipeline->layout,
      .label = pipeline->label,
      .vertex = pipeline->vertex_state,
      .primitive = pipeline->primitive_state,
      .multisample = pipeline->multisample_state,
  };

  // add optional fragment or vertex
  if (pipeline->fragment_state.module != 0)
    pipeline->descriptor.fragment = &pipeline->fragment_state;

  if (pipeline->stencil_state.format != 0)
    pipeline->descriptor.depthStencil = &pipeline->stencil_state;

  if (pipeline->handle)
    render_pipeline_destroy(pipeline);

  pipeline->handle =
      wgpuDeviceCreateRenderPipeline(context_device(), &pipeline->descriptor);
}

/**
   Release pipeline and set back the handle to null
 */
void render_pipeline_destroy(RenderPipeline *pipeline) {

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
