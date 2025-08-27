#ifndef _SHADER_STORAGE_BUFFER_OBJECT_H_
#define _SHADER_STORAGE_BUFFER_OBJECT_H_

#include "../utils/stli.h"
#include <webgpu/webgpu.h>

#define SSBO_TYPE_COUNT 7
#define SSBO_CAPACITY 32
#define SSBO_MAX_TYPE_SIZE 2048

typedef enum {
  SSBOStatus_Success,
  SSBOStatus_OutOfBound,
  SSBOStatus_UndefError,
} SSBOStatus;

typedef enum {
  SSBOType_View,
  SSBOType_Projection,
  SSBOType_Mesh,
  SSBOType_ProbeReflection,
  SSBOType_PointLight,
  SSBOType_SunLight,
  SSBOType_SpotLight,
} SSBOType;
 
typedef struct {
  uint8_t entries[SSBO_CAPACITY * SSBO_MAX_TYPE_SIZE];
  size_t capacity;
  size_t length;
  WGPUBuffer buffer;
  size_t type_size;
} __attribute__((aligned(256))) SSBO;

typedef struct {
  SSBO buffers[SSBO_TYPE_COUNT];
  WGPUQueue queue;
  WGPUDevice device;
} SSBOManager;

void ssbo_init(SSBOManager *, WGPUDevice, WGPUQueue);
SSBOStatus ssbo_update_entry(SSBOManager *, const SSBOType, size_t, void *);
SSBOStatus ssbo_upload_entry(SSBOManager *, const SSBOType, size_t, void *);
void ssbo_upload(SSBOManager *, const SSBOType);

void *ssbo_entry(SSBOManager *, const SSBOType, size_t);
void *ssbo_new_entry(SSBOManager *, const SSBOType, size_t *);
StaticListStatus ssbo_remove_entry(SSBOManager *, const SSBOType, size_t);

WGPUBuffer ssbo_buffer(SSBOManager *, const SSBOType);
size_t ssbo_length(SSBOManager *, const SSBOType);

#endif
