#include "ubo.h"
#include "../utils/system.h"
#include "stdbool.h"
#include <stddef.h>
#include <stdint.h>
#include <webgpu/webgpu.h>

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

static inline UBOValue *ubo_field_value(UBOManager *, const UBOField);

UBOValue *ubo_field_value(UBOManager *ubo, const UBOField field) {

  UBOValue *ubo_field_table[] = {
      // u32 fields
      [UBOField_PointLightCount] = &ubo->data.light_count.point,
      [UBOField_SunLightCount] = &ubo->data.light_count.sun,
      [UBOField_SpotLightCount] = &ubo->data.light_count.spot,
      [UBOField_AmbientLightCount] = &ubo->data.light_count.ambient,
      // f32 fields
  };

  return ubo_field_table[field];
}

UBOStatus ubo_update_entry(UBOManager *ubo, const UBOField field,
                           UBOValue value) {

  UBOValue *ubo_entry = ubo_field_value(ubo, field);

  if (ubo_entry) {
    *ubo_entry = value;
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
