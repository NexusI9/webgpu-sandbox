#ifndef _PIPELINE_SET_H_
#define _PIPELINE_SET_H_

#include "render.h"
#include "webgpu/webgpu.h"

#define RENDER_PIPELINE_SET_KEEP_MODULE 0

void render_pipeline_set_vertex(RenderPipeline *, const WGPUVertexState *);
void render_pipeline_set_fragment(RenderPipeline *, const WGPUFragmentState *);
void render_pipeline_set_color(RenderPipeline *, const WGPUColorTargetState *);
void render_pipeline_set_primitive(RenderPipeline *,
                                   const WGPUPrimitiveState *);
void render_pipeline_set_stencil(RenderPipeline *,
                                 const WGPUDepthStencilState *);
void render_pipeline_set_blend(RenderPipeline *, const WGPUBlendState *);
void render_pipeline_set_multisample(RenderPipeline *,
                                     const WGPUMultisampleState *);
void render_pipeline_set_sampling(RenderPipeline *,
                                  RenderPipelineMultisampleCount);

#endif
