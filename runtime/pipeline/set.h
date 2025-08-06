#ifndef _PIPELINE_SET_H_
#define _PIPELINE_SET_H_

#include "core.h"
#include "webgpu/webgpu.h"


void pipeline_set_vertex(Pipeline *, const WGPUVertexState);
void pipeline_set_fragment(Pipeline *, const PipelineFragmentDescriptor *);
void pipeline_set_primitive(Pipeline *, const WGPUPrimitiveState);
void pipeline_set_stencil(Pipeline *, const WGPUDepthStencilState);
void pipeline_set_blend(Pipeline*, const WGPUBlendState*);

#endif
