#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "renderer.h"
#include "webgpu/webgpu.h"

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  void *data;
  size_t size;
  WGPUBufferUsage usage;
} CreateBufferDescriptor;

typedef struct {
  WGPUDevice *device;
  uint32_t width;
  uint32_t height;
  unsigned char *data;
} CreateTextureDescriptor;

WGPUShaderModule create_shader(const WGPUDevice *, char *, const char *);
WGPUBuffer create_buffer(const CreateBufferDescriptor *);
ShaderTexture create_texture(const CreateTextureDescriptor *);
#endif
