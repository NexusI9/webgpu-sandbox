#ifndef _SHADER_UTILS_H_
#define _SHADER_UTILS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "core.h"
#include "webgpu/webgpu.h"

#define SHADER_BIND_VALID 0
#define SHADER_BIND_UNVALID 1

uint16_t shader_bind_group_entries_count(const ShaderBindGroup *);

bool shader_validate_binding(Shader *);

static inline size_t shader_device_storage_alignment(WGPUDevice device) {
  WGPUSupportedLimits limits;
  wgpuDeviceGetLimits(device, &limits);
  return limits.limits.minStorageBufferOffsetAlignment;
}

static inline size_t shader_device_uniform_alignment(WGPUDevice device) {
  WGPUSupportedLimits limits;
  wgpuDeviceGetLimits(device, &limits);
  return limits.limits.minUniformBufferOffsetAlignment;
}

#endif
