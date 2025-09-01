#ifndef _SHADER_UNIFORM_BUFFER_OBJECT_H_
#define _SHADER_UNIFORM_BUFFER_OBJECT_H_

#include <stdint.h>
#include <webgpu/webgpu.h>

typedef enum {
  UBOStatus_Success,
  UBOStatus_OutOfBound,
  UBOStatus_FieldValueUnfound,
  UBOStatus_UndefError,
} UBOStatus;

typedef enum {
  // u32 fields
  UBOField_PointLightCount,
  UBOField_SunLightCount,
  UBOField_SpotLightCount,
  UBOField_AmbientLightCount,
  UBOField_ProbeReflectionCount,
  // f32 fields
} UBOField;

typedef union {
  uint32_t u;
  float f;
} UBOValue;

typedef struct {
  UBOValue point;
  UBOValue spot;
  UBOValue sun;
  UBOValue ambient;
} UBOLightCount;

typedef struct {
  UBOValue reflection;
  UBOValue irradiance;
} UBOProbeCount;

typedef struct {
  UBOLightCount light_count;
  UBOProbeCount probe_count;
} UBOUniform;

typedef struct {

  UBOUniform data;
  WGPUBuffer handle;
  WGPUQueue queue;

} UBOManager;

void ubo_init(UBOManager *, WGPUQueue, const WGPUDevice);

UBOStatus ubo_update_entry(UBOManager *, const UBOField, UBOValue);

UBOStatus ubo_upload(UBOManager *);
WGPUBuffer ubo_buffer_handle(UBOManager *);

#endif
