#ifndef _COMPUTE_BGRA2RGBA_H_
#define _COMPUTE_BGRA2RGBA_H_

#include "./core.h"
#include "runtime/texture/core.h"
#include "utils/defines.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

typedef enum {
  BGRA2RGBAStatus_Success,
  BGRA2RGBAStatus_OutOfBound,
  BGRA2RGBAStatus_InitFail,
  BGRA2RGBAStatus_UndefError,
} BGRA2RGBAStatus;

static const int BGRA2RGBA_WORKGROUP = 8;

EXTERN_C_BEGIN

BGRA2RGBAStatus compute_pass_bgra2rgba_create(ComputePass *,
                                              const ComputePassDescriptor *);

BGRA2RGBAStatus compute_pass_bgra2rgba_update_source_texture(ComputePass *,
                                                             const WGPUTexture);

BGRA2RGBAStatus
compute_pass_bgra2rgba_update_destination_texture(ComputePass *,
                                                  const WGPUTexture);

static inline void compute_pass_bgra2rgba_destroy(ComputePass *pass) {
  compute_pass_destroy(pass);
}

BGRA2RGBAStatus
compute_pass_bgra2rgba_draw(ComputePass *); // builtin command encoder

BGRA2RGBAStatus compute_pass_bgra2rgba_draw_inline(ComputePass *,
                                                   const WGPUCommandEncoder);

EXTERN_C_END
#endif
