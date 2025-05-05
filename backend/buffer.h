#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "renderer.h"
#include "webgpu/webgpu.h"
#include <stddef.h>

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  void *data;
  size_t size;
  WGPUBufferUsage usage;
  WGPUBool mappedAtCreation;
} CreateBufferDescriptor;

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  uint32_t width;
  uint32_t height;
  unsigned char *data;
  uint8_t channels;
  WGPUTextureFormat format;
  size_t size;
} CreateTextureDescriptor;

void buffer_create_shader(WGPUShaderModule *, const WGPUDevice *, char *,
                          const char *);
void buffer_create(WGPUBuffer *, const CreateBufferDescriptor *);
void buffer_create_texture(WGPUTextureView *, const CreateTextureDescriptor *);
#endif
