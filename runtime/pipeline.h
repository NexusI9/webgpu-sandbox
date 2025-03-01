#ifndef _PIPELINE_H_
#define _PIPELINE_H_
#include "shader.h"
#include "webgpu/webgpu.h"

typedef struct {

  WGPUPipelineLayout layout;
  WGPUBindGroupLayout bindgroup_layout;
  WGPURenderPipeline pipeline;

} pipeline;

void pipeline_clear(pipeline *);
void pipeline_layout_group(pipeline *, shader *);
WGPURenderPipeline pipeline_create(pipeline *, shader *);
void pipeline_bind_group(pipeline *);
void pipeline_release(pipeline *);

#endif
