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

   .--------------------------- SHADER ----------------------------.
   |   .--------------.      .----------------.      .----------.  |
   |   |    Module    |      |  Vertex Layout |      |  Device  |  |
   |   '--------------'      '----------------'      '----------'  |
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

  WGPUDevice *device;
  WGPUShaderModule *module;
  WGPUVertexBufferLayout *vertex_layout;
  WGPURenderPipelineDescriptor descriptor;
  WGPURenderPipeline handle;
  WGPUPipelineLayout layout;
    
  WGPUVertexState vertex_state;
  WGPUFragmentState fragment_state;
  WGPUPrimitiveState primitive_state;
  WGPUDepthStencilState stencil_state;

} pipeline;

// 1. init pipeline
void pipeline_create(pipeline *, const PipelineCreateDescriptor *);

// 2. set custom attributes (optional)
void pipeline_set_vertex(pipeline *, WGPUVertexState);
void pipeline_set_fragment(pipeline *, WGPUFragmentState);
void pipeline_set_primitive(pipeline *, WGPUPrimitiveState);
void pipeline_set_stencil(pipeline *, WGPUDepthStencilState);

// 3. build pipeline layout
void pipeline_build(pipeline *, WGPUPipelineLayout *);

// 4. destroyer
void pipeline_clear(pipeline *);

#endif
