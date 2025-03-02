#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "webgpu/webgpu.h"
#include "renderer.h"

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  void *data;
  size_t size;
  WGPUBufferUsage usage;
} CreateBufferDescriptor;

WGPUShaderModule create_shader(const WGPUDevice *, char *, const char *);
WGPUBuffer create_buffer(const CreateBufferDescriptor *);

#endif
