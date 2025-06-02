#include "set.h"


/**
   Define custom vertex state for pipeline prior building it
 */
void pipeline_set_vertex(Pipeline *pipeline, const WGPUVertexState state) {
  pipeline->vertex_state = state;
}

/**
   Define custom fragment state for pipeline prior building it
 */
void pipeline_set_fragment(Pipeline *pipeline,
                           const PipelineFragmentDescriptor *state) {

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
void pipeline_set_primitive(Pipeline *pipeline,
                            const WGPUPrimitiveState state) {
  pipeline->primitive_state = state;
}

/**
   Define custom stencil state for pipeline prior building it
 */
void pipeline_set_stencil(Pipeline *pipeline,
                          const WGPUDepthStencilState state) {
  pipeline->stencil_state = state;
}

