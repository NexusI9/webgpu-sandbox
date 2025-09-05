#ifndef _SHADER_UNIFORM_BUFFER_OBJECT_H_
#define _SHADER_UNIFORM_BUFFER_OBJECT_H_

#include "../runtime/scene/environment/fog.h"
#include "../runtime/light/list.h"
#include "../runtime/probe/core.h"

#include <cglm/cglm.h>
#include <stdint.h>
#include <webgpu/webgpu.h>

typedef enum {
  UBOStatus_Success,
  UBOStatus_OutOfBound,
  UBOStatus_FieldValueUnfound,
  UBOStatus_UndefError,
} UBOStatus;

#define UBO_FIELD_COUNT 7

typedef enum {
  // u32 fields
  UBOField_PointLightCount,
  UBOField_SunLightCount,
  UBOField_SpotLightCount,
  UBOField_AmbientLightCount,
  UBOField_ProbeReflectionGridCount,
  UBOField_ProbeReflectionPlaneCount,
  // f32 fields

  // struct fields
  UBOField_Fog
} UBOField;


typedef struct {
  LightCountUniform light_count;
  ProbeCountUniform probe_count;
  SceneEnvironmentFogUniform fog;
} UBOUniform;

typedef struct {
  UBOUniform data;
  WGPUBuffer handle;
  WGPUQueue queue;
} UBOManager;

void ubo_init(UBOManager *, WGPUQueue, const WGPUDevice);

UBOStatus ubo_update_entry(UBOManager *, const UBOField, void *);

UBOStatus ubo_upload(UBOManager *);
WGPUBuffer ubo_buffer_handle(UBOManager *);

#endif
