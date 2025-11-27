#ifndef _COMPUTE_CORE_H_
#define _COMPUTE_CORE_H_

#include "runtime/texture/core.h"
#include "utils/defines.h"
#include <stdint.h>
#include <webgpu/webgpu.h>

static const uint8_t COMPUTE_PASS_BUFFER_CAPACITY = 6;
static const uint8_t COMPUTE_PASS_BINDGROUP_CAPACITY = 128;
static const uint8_t COMPUTE_PASS_VIEW_CAPACITY = 128;

typedef enum {
  ComputePassStatus_Success,
  ComputePassStatus_InvalidSlot,
  ComputePassStatus_OutOfBound,
  ComputePassStatus_UndefError,
} ComputePassStatus;

typedef struct {

  const char *label;
  int width, height;
  uint32_t layer_count;

  WGPUBuffer buffers[COMPUTE_PASS_BUFFER_CAPACITY];
  WGPUBindGroup bindgroups[COMPUTE_PASS_BINDGROUP_CAPACITY];
  WGPUTextureView views[COMPUTE_PASS_VIEW_CAPACITY];
  WGPUTexture source_texture, buffer_texture;
  WGPUSampler sampler;

} ComputePass;

typedef struct {
  const char *label;
  WGPUTexture source_texture, buffer_texture;
} ComputePassDescriptor;

EXTERN_C_BEGIN

ComputePassStatus compute_pass_create(ComputePass *,
                                      const ComputePassDescriptor *);

ComputePassStatus compute_pass_destroy(ComputePass *);

static inline void compute_pass_dispatch(
    WGPUComputePassEncoder pass, const uint32_t workgroup,
    const TextureResolution width, const TextureResolution height) {

  uint32_t dispatch_x = (width + workgroup - 1) / workgroup;
  uint32_t dispatch_y = (height + workgroup - 1) / workgroup;

  wgpuComputePassEncoderDispatchWorkgroups(pass, dispatch_x, dispatch_y, 1);
}

EXTERN_C_END

#endif
