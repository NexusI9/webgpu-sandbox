#ifndef _PIPELINE_CORE_H_
#define _PIPELINE_CORE_H_

#include <stddef.h>
#include <stdint.h>

#include "../runtime/geometry/vertex/vertex.h"
#include "webgpu/webgpu.h"
#include "../runtime/geometry/vertex/core.h"

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
#define PIPELINE_TYPE_COUNT 14

typedef enum {
  PipelineType_Billboard,
  PipelineType_Default,
  PipelineType_Grid,
  PipelineType_Line,
  PipelineType_PBR,
  PipelineType_Screen,
  PipelineType_Shadow,
  PipelineType_ShadowCullBack,
  PipelineType_Skybox,
  PipelineType_Solid,
  PipelineType_Unlit,
  PipelineType_GlassProbeGrid,
  PipelineType_GlassProbePlane,
  PipelineType_Reflection,
} PipelineType;

typedef struct {
  WGPUFragmentState fragment_state;
  WGPUColorTargetState color_state;
  WGPUBlendState blend_state;
} PipelineFragmentDescriptor;

#define PIPELINE_BINDING_UNDEFINED UINT16_MAX

typedef struct {
  uint8_t group;
  uint16_t model;
  uint16_t view;
  uint16_t projection;
} PipelineBindingMVP;

typedef struct {
  uint8_t group;
  uint16_t ambient;
  uint16_t spot;
  uint16_t point;
  uint16_t sun;
  uint16_t point_texture;
  uint16_t directional_texture;
} PipelineBindingLightList;

typedef struct {
  uint8_t group;

  uint16_t reflection_plane;
  uint16_t reflection_plane_texture;
  uint16_t reflection_plane_sampler;

  uint16_t reflection_grid;
  uint16_t reflection_grid_texture;
  uint16_t reflection_grid_sampler;

  uint16_t irradiance;
  uint16_t irradiance_texture;
  uint16_t irradiance_sampler;

  uint16_t skybox_texture;
  uint16_t skybox_sampler;

} PipelineBindingProbe;

typedef struct {
  const PipelineBindingMVP *mvp;
  const PipelineBindingLightList *light_list;
  const PipelineBindingProbe *probe;
} PipelineBinding;

typedef struct {
  const char *label;

  // WGSL file to load module
  const char *shader_path;
  const WGPUBindGroupLayoutDescriptor *bind_groups[4];

  // Shader bind groups layout
  size_t bind_groups_count;

  struct {
    WGPUVertexState vertex_state;
    PipelineFragmentDescriptor fragment_state;
    WGPUPrimitiveState primitive_state;
    WGPUDepthStencilState stencil_state;
    WGPUBlendState blend_state;
    WGPUMultisampleState multisample_state;
  } pipeline_attributes;

  PipelineBinding bindings;

} ShaderPipelineStateObject;

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
  WGPUMultisampleState multisample_state;

  // layout
  WGPURenderPipelineDescriptor descriptor;
  WGPURenderPipeline handle;
  WGPUPipelineLayout layout;

  // initial layout
  const ShaderPipelineStateObject *shader_pso;

  // pipeline std bindings
  PipelineBinding bindings;

  // vertex data
  struct {
    WGPUVertexAttribute attribute[VERTEX_ATTRIBUTE_COUNT];
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
