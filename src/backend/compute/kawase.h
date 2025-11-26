#ifndef _COMPUTE_KAWASE_H_
#define _COMPUTE_KAWASE_H_

#include "./core.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

typedef enum {
  KawaseStatus_Success,
  KawaseStatus_OutOfBound,
  KawaseStatus_InitFail,
  KawaseStatus_UndefError,
} KawaseStatus;

static const int KAWASE_WORKGROUP = 8;
static const int KAWASE_BUFFER_UNIFORM = 0;

typedef struct {
  vec2 texel_size;
  float offset;
  float _pad;
  vec4 _pad2;
} KawaseUniform;

KawaseStatus compute_pass_kawase_create(ComputePass *,
                                        const ComputePassDescriptor *);

KawaseStatus compute_pass_kawase_update_source_texture(ComputePass *,
                                                       const WGPUTexture);

static inline void compute_pass_kawase_destroy(ComputePass *pass) {
  compute_pass_destroy(pass);
}

KawaseStatus
compute_pass_kawase_draw(ComputePass *,
                         const uint32_t); // builtin command encoder

KawaseStatus compute_pass_kawase_draw_inline(ComputePass *, const uint32_t,
                                             const WGPUCommandEncoder);

#endif
