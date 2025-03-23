#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include "webgpu/webgpu.h"

#define PIPELINE_DEFAULT_CULLMODE WGPUCullMode_Back;

typedef struct {
  WGPUCullMode cullMode;
} PipelineCustomAttributes;

typedef struct {
  WGPUDevice *device;
  WGPUShaderModule *module;
  WGPUVertexBufferLayout *vertex_layout;
  PipelineCustomAttributes *custom;
} PipelineCreateDescriptor;

typedef struct {

  WGPUDevice *device;
  WGPUShaderModule *module;
  WGPUVertexBufferLayout *vertex_layout;
  WGPURenderPipelineDescriptor descriptor;
  WGPURenderPipeline handle;
  WGPUPipelineLayout layout;
  PipelineCustomAttributes *custom_attributes;

} pipeline;

void pipeline_create(pipeline *, const PipelineCreateDescriptor *);
void pipeline_set_custom(pipeline *, PipelineCustomAttributes *);
void pipeline_build(pipeline *, WGPUPipelineLayout *);
void pipeline_clear(pipeline *);
#endif
