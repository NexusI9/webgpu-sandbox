#ifndef _SHADER_STORAGE_BUFFER_OBJECT_H_
#define _SHADER_STORAGE_BUFFER_OBJECT_H_

#include <webgpu/webgpu.h>

#define SSBO_TYPE_COUNT 5
#define SSBO_ENTRY_CAPACITY 32
#define SSBO_MAX_TYPE_SIZE 2048

typedef enum {
  SSBOStatus_Success,
  SSBOStatus_OutOfBound,
  SSBOStatus_UndefError,
} SSBOStatus;

typedef enum {
  SSBOType_View,
  SSBOType_Projection,
  SSBOType_PointLight,
  SSBOType_SunLight,
  SSBOType_SpotLight,
} SSBOType;

typedef struct {
  uint8_t entries[SSBO_ENTRY_CAPACITY * SSBO_MAX_TYPE_SIZE];
  WGPUBuffer buffer;
  size_t type_size;
} __attribute__((aligned(16))) SSBO;

typedef struct {
  SSBO buffers[SSBO_TYPE_COUNT];
  WGPUQueue queue;
  WGPUDevice device;
} SSBOManager;

void ssbo_init(SSBOManager *, WGPUDevice, WGPUQueue);
SSBOStatus ssbo_update_entry(SSBOManager *, const SSBOType, size_t, void *);
SSBOStatus ssbo_upload_entry(SSBOManager *, const SSBOType, size_t, void *);
void ssbo_upload(SSBOManager *, const SSBOType);

#endif
