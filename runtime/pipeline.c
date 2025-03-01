#include "pipeline.h"
#include "webgpu/webgpu.h"

// Release pipeline from GPU
void pipeline_clear(pipeline *pipeline) {
  wgpuRenderPipelineRelease(pipeline->pipeline);
}

void pipeline_layout_group(pipeline *pipeline, shader *shader) {}

WGPURenderPipeline pipeline_create(pipeline *pipeline, shader *shader) {

  return pipeline->pipeline;
}

void pipeline_bind_group(pipeline *pipeline) {}

void pipeline_release(pipeline *pipeline) {}
