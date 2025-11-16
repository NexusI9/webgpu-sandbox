#ifndef _COMPUTE_KAWASE_H_
#define _COMPUTE_KAWASE_H_

#include "./core.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

#define KAWASE_WORKGROUP 8

typedef enum {
  KawaseStatus_Success,
  KawaseStatus_UndefError,
} KawaseStatus;

typedef struct {
  vec2 texel_size;
  float offset;
  float _pad;
  vec4 _pad2;
} KawaseUniform;

typedef struct {
  WGPUTexture texture;
  const size_t layer_count;
  const uint32_t pass_count;
} KawaseDescriptor;

KawaseStatus
compute_pass_kawase(ComputePass *,
                    const KawaseDescriptor *); // builtin command encoder

KawaseStatus compute_pass_kawase_inline(ComputePass *, const KawaseDescriptor *,
                                        const WGPUCommandEncoder);

#endif
