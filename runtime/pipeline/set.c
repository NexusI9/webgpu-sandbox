#include "set.h"

#include "webgpu/webgpu.h"

/**
   Define custom vertex state for pipeline prior building it
 */
void render_pipeline_set_vertex(RenderPipeline *pipeline,
                                const WGPUVertexState state) {
  pipeline->vertex_state = state;
}

/**
   Define custom fragment state for pipeline prior building it
 */
void render_pipeline_set_fragment(
    RenderPipeline *pipeline, const RenderPipelineFragmentDescriptor *state) {

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
void render_pipeline_set_primitive(RenderPipeline *pipeline,
                                   const WGPUPrimitiveState state) {
  pipeline->primitive_state = state;
}

/**
   Define custom stencil state for pipeline prior building it
 */
void render_pipeline_set_stencil(RenderPipeline *pipeline,
                                 const WGPUDepthStencilState state) {
  pipeline->stencil_state = state;
}

void render_pipeline_set_blend(RenderPipeline *pipeline,
                               const WGPUBlendState *state) {

  pipeline->blend_state = (WGPUBlendState){
      .color = state->color,
      .alpha = state->alpha,
  };

  pipeline->color_state.blend = &pipeline->blend_state;
  pipeline->fragment_state.targets = &pipeline->color_state;
}

void render_pipeline_set_multisample(RenderPipeline *pipeline,
                                     const WGPUMultisampleState *state) {
  pipeline->multisample_state = *state;
}


void render_pipeline_set_sampling(RenderPipeline *pipeline,
                           RenderPipelineMultisampleCount sampling) {
  pipeline->multisample_state.count = sampling;
}
