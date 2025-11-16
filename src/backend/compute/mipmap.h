#ifndef _COMPUTE_MIPMAP_H_
#define _COMPUTE_MIPMAP_H_

#include "core.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"

#define MIPMAP_WORKGROUP 8

typedef enum {
  MipmapStatus_Success,
  MipmapStatus_UndefError,
} MipmapStatus;

typedef uint32_t mip_t;

typedef struct {
  WGPUTexture texture;
  const size_t layer_count;
} MipmapDescriptor;

MipmapStatus compute_pass_mipmap(ComputePass *, const MipmapDescriptor *);

static inline mip_t mipmap_count(const TextureResolution width,
                                 const TextureResolution height) {
  return floorf(log2f(glm_max(width, height))) + 1;
}
#endif
