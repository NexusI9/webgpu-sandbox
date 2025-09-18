#ifndef _COMPUTE_CORE_H_
#define _COMPUTE_CORE_H_

#include <webgpu/webgpu.h>

#define COMPUTE_PASS_BUFFER_MAX_SIZE 2048

typedef enum {
  ComputePassStatus_Success,
  ComputePassStatus_UndefError,
} ComputePassStatus;

typedef struct {

  WGPUDevice device;
  WGPUQueue queue;
  int max_width;
  int max_height;

  WGPUBuffer buffer;
  WGPUTexture destination_texture;
  WGPUSampler sampler;

} ComputePass;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  const int max_width;
  const int max_height;
} ComputePassDescriptor;

ComputePassStatus compute_pass_init(ComputePass *,
                                    const ComputePassDescriptor *);

#endif
