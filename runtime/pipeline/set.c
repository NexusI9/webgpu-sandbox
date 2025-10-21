#include "set.h"

#include "webgpu/webgpu.h"

/**
   Define custom vertex state for pipeline prior building it
 */
void render_pipeline_set_vertex(RenderPipeline *pipeline,
                                const WGPUVertexState* state) {

  WGPUShaderModule tmp_module = pipeline->vertex_state.module;
  pipeline->vertex_state = *state;

  if (state->module == RENDER_PIPELINE_SET_KEEP_MODULE)
    pipeline->vertex_state.module = tmp_module;
}

/**
   Define custom fragment state for pipeline prior building it
 */
void render_pipeline_set_fragment(RenderPipeline *pipeline,
                                  const WGPUFragmentState *state) {
  pipeline->fragment_state = *state;
}

void render_pipeline_set_color(RenderPipeline *pipeline,
                               const WGPUColorTargetState *state) {

  pipeline->color_state = *state;

  if (pipeline->color_state.blend == RENDER_PIPELINE_SET_KEEP_BLEND)
    pipeline->color_state.blend = &pipeline->blend_state;

  pipeline->fragment_state.targets = &pipeline->color_state;
}

/**
   Define custom primitive state for pipeline prior building it
 */
void render_pipeline_set_primitive(RenderPipeline *pipeline,
                                   const WGPUPrimitiveState *state) {
  pipeline->primitive_state = *state;
}

/**
   Define custom stencil state for pipeline prior building it
 */
void render_pipeline_set_stencil(RenderPipeline *pipeline,
                                 const WGPUDepthStencilState* state) {
  pipeline->stencil_state = *state;
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
