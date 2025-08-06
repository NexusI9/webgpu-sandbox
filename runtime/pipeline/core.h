#ifndef _PIPELINE_CORE_H_
#define _PIPELINE_CORE_H_

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

typedef enum {
  PipelineMultisampleCount_Undefined = 0,
  PipelineMultisampleCount_1x = 1,
  PipelineMultisampleCount_4x = 4
} PipelineMultisampleCount;

typedef struct {
  WGPUDevice device;
  const char *label;
  const char *path;
} PipelineCreateDescriptor;

// Standards pipelines
#define PIPELINE_TYPE_COUNT 10

typedef enum {
  PipelineType_Billboard,
  PipelineType_Default,
  PipelineType_Grid,
  PipelineType_Line,
  PipelineType_PBR,
  PipelineType_Screen,
  PipelineType_Shadow,
  PipelineType_Skybox,
  PipelineType_Solid,
  PipelineType_Unlit,
} PipelineType;


typedef struct {
  WGPUFragmentState fragment_state;
  WGPUColorTargetState color_state;
  WGPUBlendState blend_state;
} PipelineFragmentDescriptor;

typedef struct {
  const char *label;

  // WGSL file to load module
  const char *shader_path;

  // Shader bind groups layout
  size_t bind_groups_count;
  const WGPUBindGroupLayoutDescriptor *bind_groups;

  struct {
    WGPUVertexState vertex_state;
    PipelineFragmentDescriptor fragment_state;
    WGPUPrimitiveState primitive_state;
    WGPUDepthStencilState stencil_state;
    WGPUBlendState blend_state;
    PipelineMultisampleCount multisample;
  } custom_attributes;

} PipelineLayoutDescriptor;

typedef struct {

  const char *label;
  const char *path;

  // core
  WGPUDevice device;
  WGPUShaderModule module;

  // cached attributes
  WGPUVertexState vertex_state;
  WGPUFragmentState fragment_state;
  WGPUPrimitiveState primitive_state;
  WGPUDepthStencilState stencil_state;
  WGPUColorTargetState color_state;
  WGPUBlendState blend_state;
  PipelineMultisampleCount sampling;

  // layout
  WGPURenderPipelineDescriptor descriptor;
  WGPURenderPipeline handle;
  WGPUPipelineLayout layout;

  // vertex data
  struct {
    WGPUVertexAttribute attribute[4];
    WGPUVertexBufferLayout buffer;
  } vertex_layout;

} Pipeline;

void pipeline_standards_create(Pipeline *);

// init pipeline
void pipeline_create(Pipeline *, const PipelineCreateDescriptor *);

// build pipeline layout
void pipeline_build(Pipeline *, const WGPUPipelineLayout *);

// destroyer
void pipeline_destroy(Pipeline *);

void pipeline_set_sampling(Pipeline *, PipelineMultisampleCount);

#endif
