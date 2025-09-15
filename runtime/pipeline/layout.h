#ifndef _PIPELINE_LAYOUT_H_
#define _PIPELINE_LAYOUT_H_

#include <webgpu/webgpu.h>
#include <stddef.h>

#include "./core.h"

WGPUPipelineLayout shader_pipeline_state_object_create(
    const WGPUBindGroupLayoutDescriptor *const *, const size_t,
    const WGPUDevice, WGPUBindGroupLayout *outLayout);

#endif
