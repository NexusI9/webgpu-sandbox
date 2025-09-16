#ifndef _MIPMAPS_CORE_H_
#define _MIPMAPS_CORE_H_

#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"

typedef enum {
  MipmapsStatus_Success,
  MipmapsStatus_UndefError,
} MipmapsStatus;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  const TextureResolution width;
  const TextureResolution height;
  const WGPUTextureViewDimension dimension;
} MipmapsCreateDescriptor;

MipmapsStatus mipmaps_create(WGPUTexture *, const MipmapsCreateDescriptor *);
#endif
