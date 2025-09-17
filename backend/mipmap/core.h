#ifndef _MIPMAP_CORE_H_
#define _MIPMAP_CORE_H_

#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"

#define MIPMAP_WORKGROUP 8

typedef enum {
  MipmapStatus_Success,
  MipmapStatus_UndefError,
} MipmapStatus;

typedef uint32_t mip_t;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  const WGPUTextureViewDimension dimension;
  const WGPUTextureFormat format;
  const TextureResolution width;
  const TextureResolution height;
} MipmapCreateDescriptor;

MipmapStatus mipmap_create(WGPUTexture, const MipmapCreateDescriptor *);

static inline mip_t mipmap_count(const TextureResolution width,
                                 const TextureResolution height) {
  return floorf(log2f(glm_max(width, height))) + 1;
}
#endif
