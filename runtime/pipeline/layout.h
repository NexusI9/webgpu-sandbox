#ifndef _PIPELINE_LAYOUT_H_
#define _PIPELINE_LAYOUT_H_

#include "./core.h"
#include <webgpu/webgpu.h>

WGPUPipelineLayout
pipeline_layout_descriptor_create(const WGPUBindGroupLayoutDescriptor *,
                                  const size_t, const WGPUDevice,
                                  WGPUBindGroupLayout *outLayout);

#endif
