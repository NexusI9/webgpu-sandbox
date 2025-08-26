#include "ssbo.h"
#include "../runtime/camera/camera.h"
#include "../runtime/light/light.h"
#include "../runtime/viewport/viewport.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

static const struct {
  const size_t size;
  const char *label;
} ssbo_type[SSBO_TYPE_COUNT] = {
    [SSBOType_View] =
        {
            sizeof(CameraUniform),
            "SSBO View Buffer",
        },
    [SSBOType_Projection] =
        {
            sizeof(ViewportUniform),
            "SSBO Projection Buffer",
        },
    [SSBOType_PointLight] =
        {
            sizeof(PointLightListUniform),
            "SSBO Point Light List Buffer",
        },
    [SSBOType_SunLight] =
        {
            sizeof(SunLightListUniform),
            "SSBO Sun Light List Buffer",
        },
    [SSBOType_SpotLight] =
        {
            sizeof(SpotLightListUniform),
            "SSBO Spot Light List Buffer",
        },
};

void ssbo_init(SSBOManager *manager, WGPUDevice device, WGPUQueue queue) {

  VERBOSE_PROCESS("Initializing SSBO Manager");

  manager->device = device;
  manager->queue = queue;

  for (SSBOType i = 0; i < SSBO_TYPE_COUNT; i++) {
    manager->buffers[i].type_size = ssbo_type[i].size;
    manager->buffers[i].buffer = wgpuDeviceCreateBuffer(
        device, &(WGPUBufferDescriptor){
                    .size = SSBO_ENTRY_CAPACITY * SSBO_MAX_TYPE_SIZE,
                    .mappedAtCreation = false,
                    .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
                    .label = ssbo_type[i].label,
                });
  }
}

SSBOStatus ssbo_update_entry(SSBOManager *manager, const SSBOType type,
                             size_t index, void *data) {

  if (index >= SSBO_ENTRY_CAPACITY) {
    VERBOSE_WARNING(
        "Attempting to write into SSBO out of bound index (%lu) max SSBO "
        "capacity is currently set to %d.",
        index, SSBO_ENTRY_CAPACITY);
    return SSBOStatus_OutOfBound;
  }

  SSBO *ssbo = &manager->buffers[type];
  memcpy((void *)ssbo->entries + index * ssbo->type_size, data,
         ssbo->type_size);

  return SSBOStatus_Success;
}

SSBOStatus ssbo_upload_entry(SSBOManager *manager, const SSBOType type,
                             size_t index, void *data) {

  // first update stagging
  SSBOStatus stagging_udpate = ssbo_update_entry(manager, type, index, data);

  if (stagging_udpate == SSBOStatus_Success) {

    // update ssbo buffer at index
    SSBO *ssbo = &manager->buffers[type];
    size_t offset = index * ssbo->type_size;
    wgpuQueueWriteBuffer(manager->queue, ssbo->buffer, offset,
                         ssbo->entries + offset, ssbo->type_size);
  }

  return stagging_udpate;
}

void ssbo_upload(SSBOManager *manager, const SSBOType type) {

  SSBO *ssbo = &manager->buffers[type];
  wgpuQueueWriteBuffer(manager->queue, ssbo->buffer, 0, ssbo->entries,
                       ssbo->type_size * SSBO_ENTRY_CAPACITY);
}
