#include "ubo.h"
#include "../utils/system.h"
#include "stdbool.h"
#include "string.h"
#include <stddef.h>
#include <stdint.h>
#include <webgpu/webgpu.h>

typedef struct {
  void *data;
  size_t type_size;
} UBOEntry;

void ubo_init(UBOManager *ubo, WGPUQueue queue, const WGPUDevice device) {
  ubo->queue = queue;
  ubo->handle = wgpuDeviceCreateBuffer(
      device, &(WGPUBufferDescriptor){
                  .size = sizeof(UBOUniform),
                  .mappedAtCreation = false,
                  .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
                  .label = "UBO",
              });
}

UBOStatus ubo_upload(UBOManager *ubo) {
  wgpuQueueWriteBuffer(ubo->queue, ubo->handle, 0, &ubo->data,
                       sizeof(UBOUniform));
  return UBOStatus_Success;
}

WGPUBuffer ubo_buffer_handle(UBOManager *ubo) { return ubo->handle; }

static inline UBOStatus ubo_field_entry(UBOManager *, const UBOField,
                                        UBOEntry *);

UBOStatus ubo_field_entry(UBOManager *ubo, const UBOField field,
                          UBOEntry *endpoint) {

  if (field > UBO_FIELD_COUNT) {
    VERBOSE_WARNING("Attempting to alter an out of bound UBO Field (%d).",
                    field);
    return UBOStatus_OutOfBound;
  }

  UBOEntry ubo_field_table[UBO_FIELD_COUNT] = {
      // u32 fields
      [UBOField_PointLightCount] =
          {
              .data = &ubo->data.light_count.point,
              .type_size = sizeof(uint32_t),
          },
      [UBOField_SunLightCount] =
          {
              .data = &ubo->data.light_count.sun,
              .type_size = sizeof(uint32_t),
          },
      [UBOField_SpotLightCount] =
          {
              .data = &ubo->data.light_count.spot,
              .type_size = sizeof(uint32_t),
          },
      [UBOField_AmbientLightCount] =
          {
              .data = &ubo->data.light_count.ambient,
              .type_size = sizeof(uint32_t),
          },
      [UBOField_ProbeReflectionGridCount] =
          {
              .data = &ubo->data.probe_count.reflection_grid,
              .type_size = sizeof(uint32_t),
          },
      [UBOField_ProbeReflectionPlaneCount] =
          {
              .data = &ubo->data.probe_count.reflection_plane,
              .type_size = sizeof(uint32_t),
          },
      // f32 fields
      // struct fields
      [UBOField_Fog] =
          {
              .data = &ubo->data.fog,
              .type_size = sizeof(SceneEnvironmentFogUniform),
          },

  };

  endpoint->data = ubo_field_table[field].data;
  endpoint->type_size = ubo_field_table[field].type_size;

  return UBOStatus_Success;
}

UBOStatus ubo_update_entry(UBOManager *ubo, const UBOField field, void *value) {

  UBOEntry ubo_entry;
  ubo_field_entry(ubo, field, &ubo_entry);

  if (ubo_entry.data) {
    memcpy(ubo_entry.data, value, ubo_entry.type_size);
    return UBOStatus_Success;
  } else {
    VERBOSE_WARNING("UBO requested value for field %d returned NULL. This "
                    "means that either "
                    "you are attempting to reach and out of bound field index "
                    "or that the lookup "
                    "UBO hasn't a value for this field yet.",
                    field);
    return UBOStatus_FieldValueUnfound;
  }
}
