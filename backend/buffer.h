#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "webgpu/webgpu.h"

typedef enum {
  BufferTextureMemory_Keep,
  BufferTextureMemory_Free,
} BufferTextureMemory;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  void *data;
  size_t size;
  WGPUBufferUsage usage;
  WGPUBool mappedAtCreation;
  const char *label;
} CreateBufferDescriptor;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  uint32_t width;
  uint32_t height;
  unsigned char *data;
  uint8_t channels;
  WGPUTextureFormat format;
  size_t size;
} CreateTextureDescriptor;

typedef struct {
  const WGPUQueue queue;
  const WGPUTexture texture;
  unsigned char *data;
  uint32_t width;
  uint32_t height;
  uint32_t layer;
  uint8_t channels;
  WGPUTextureFormat format;
  size_t size;
} CreateTextureCubeDescriptor;

void buffer_create_shader(WGPUShaderModule *, const WGPUDevice, char *,
                          const char *);
void buffer_create(WGPUBuffer *, const CreateBufferDescriptor *);

void buffer_create_texture(WGPUTexture*, WGPUTextureView *, const CreateTextureDescriptor *,
                           BufferTextureMemory);

void buffer_create_texture_cube(const CreateTextureCubeDescriptor *,
                                BufferTextureMemory);
#endif
