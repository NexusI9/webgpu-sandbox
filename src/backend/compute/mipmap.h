#ifndef _COMPUTE_MIPMAP_H_
#define _COMPUTE_MIPMAP_H_

#include "core.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

#define MIPMAP_WORKGROUP 8

typedef enum {
  MipmapStatus_Success,
  MipmapStatus_OutOfBound,
  MipmapStatus_InitFail,
  MipmapStatus_UndefError,
} MipmapStatus;

typedef uint32_t mip_t;

MipmapStatus compute_pass_mipmap_create(ComputePass *,
                                        const ComputePassDescriptor *);

MipmapStatus compute_pass_mipmap_update_source_texture(ComputePass *,
                                                       const WGPUTexture);

void compute_pass_mipmap_draw(ComputePass *);

static inline void compute_pass_mipmap_destroy(ComputePass *pass) {
  compute_pass_destroy(pass);
}

static inline mip_t mipmap_count(const TextureResolution width,
                                 const TextureResolution height) {
  return floorf(log2f(glm_max(width, height))) + 1;
}
#endif
