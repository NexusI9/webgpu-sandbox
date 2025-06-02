#ifndef _PIPELINE_SET_H_
#define _PIPELINE_SET_H_

#include "core.h"

typedef struct {
  WGPUFragmentState fragment_state;
  WGPUColorTargetState color_state;
  WGPUBlendState blend_state;
} PipelineFragmentDescriptor;

void pipeline_set_vertex(Pipeline *, const WGPUVertexState);
void pipeline_set_fragment(Pipeline *, const PipelineFragmentDescriptor *);
void pipeline_set_primitive(Pipeline *, const WGPUPrimitiveState);
void pipeline_set_stencil(Pipeline *, const WGPUDepthStencilState);

#endif
