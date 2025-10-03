#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include "webgpu/webgpu.h"
typedef enum {
  REMStatus_Success,
  REMStatus_MaxCapacity,
  REMStatus_UndefError,
} REMStatus;

typedef struct {

} ResourceManager;

WGPUTexture rem_create_texture(ResourceManager *, WGPUTextureDescriptor);
WGPUTextureView rem_create_view(ResourceManager *, WGPUTextureViewDescriptor);

#endif
