#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "state.h"
#include "webgpu/webgpu.h"

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  void *data;
  size_t size;
  WGPUBufferUsage usage;
} CreateBufferDescriptor;

WGPUSwapChain create_swapchain(const state_t *);
WGPUShaderModule create_shader(const WGPUDevice *, const char *, const char *);
WGPUBuffer create_buffer(const CreateBufferDescriptor *);

#endif
