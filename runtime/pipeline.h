#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include "webgpu/webgpu.h"

/**
   ============================== PIPELINE ==============================

   Provide functions to create a pipeline and edit it.
   Since pipelines are differents depending on their purpose (depth/ color)
   It's hard to define a preset/ global pipeline for each objects or pass.
   Thus we need to provide a way to systematize de pipeline creation
   without loosing the customization aspect.

   The overall flow is as follow:

   .---------------------------- SHADER ---------------------------.
   |  .--------------.      .----------------.      .----------.   |
   |  |    Module    |      |  Vertex Layout |      |  Device  |   |
   |  '--------------'      '----------------'      '----------'   |
   '---------------------------------------------------------------'
                                   |
                         .-------------------.
                         |  Pipeline Create  |
                         '-------------------'
                                   |
                         .-------------------.
                         |   Pipeline Edit   |
                         | .---------------. |
                         | |     Vertex    | |
                         | '---------------' |
                         | .---------------. |
                         | |   Fragment    | |
                         | '---------------' |
                         | .---------------. |
                         | |   Primitive   | |
                         | '---------------' |
                         | .---------------. |
                         | |    Stencil    | |
                         | '---------------' |
                         '-------------------'
                                   |
                        (Bind Shader groups...)
                                   |
                         .-------------------.      .------ SHADER ------.
                         |   Pipeline Build  | <----|  Bind group layout |
                         '-------------------'      '--------------------'
 */

typedef struct {
  WGPUDevice *device;
  WGPUShaderModule *module;
  WGPUVertexBufferLayout *vertex_layout;
} PipelineCreateDescriptor;

typedef struct {
  WGPUFragmentState fragment_state;
  WGPUColorTargetState color_state;
  WGPUBlendState blend_state;
} PipelineFragmentDescriptor;

typedef struct {

  // core
  WGPUDevice *device;
  WGPUShaderModule *module;
  WGPUVertexBufferLayout *vertex_layout;

  // cached attributes
  WGPUVertexState vertex_state;
  WGPUFragmentState fragment_state;
  WGPUPrimitiveState primitive_state;
  WGPUDepthStencilState stencil_state;
  WGPUColorTargetState color_state;
  WGPUBlendState blend_state;

  // layout
  WGPURenderPipelineDescriptor descriptor;
  WGPURenderPipeline handle;
  WGPUPipelineLayout layout;

} pipeline;

// 1. init pipeline
void pipeline_create(pipeline *, const PipelineCreateDescriptor *);

// 2. set custom attributes (optional)
void pipeline_set_vertex(pipeline *, const WGPUVertexState);
void pipeline_set_fragment(pipeline *, const PipelineFragmentDescriptor *);
void pipeline_set_primitive(pipeline *, const WGPUPrimitiveState);
void pipeline_set_stencil(pipeline *, const WGPUDepthStencilState);

// 3. build pipeline layout
void pipeline_build(pipeline *, const WGPUPipelineLayout *);

// 4. destroyer
void pipeline_destroy(pipeline *);

#endif
