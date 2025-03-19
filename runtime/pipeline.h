#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include "webgpu/webgpu.h"

typedef struct {
  WGPUDevice *device;
  WGPUShaderModule *module;
  WGPUPipelineLayout layout;
  WGPUVertexBufferLayout *vertex_layout;
} PipelineCreateDescriptor;

typedef struct {

  WGPUDevice *device;
  WGPUShaderModule *module;
  WGPUVertexBufferLayout *vertex_layout;
  WGPURenderPipelineDescriptor descriptor;
  WGPURenderPipeline handle;
  WGPUPipelineLayout layout;

} pipeline;

void pipeline_create_default(pipeline *, const PipelineCreateDescriptor *);
void pipeline_set_cullmode(pipeline *, WGPUCullMode);

#endif
