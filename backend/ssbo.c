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
    [SSBOType_Mesh] =
        {
            sizeof(MeshUniform),
            "SSBO Mesh Buffer",
        },
    [SSBOType_ProbeReflection] =
        {
            sizeof(ViewportUniform),
            "SSBO Mesh Buffer",
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
    SSBO *ssbo = &manager->buffers[i];
    ssbo->type_size = ssbo_type[i].size;
    ssbo->length = 0;
    ssbo->capacity = SSBO_CAPACITY * SSBO_MAX_TYPE_SIZE;
    ssbo->buffer = wgpuDeviceCreateBuffer(
        device, &(WGPUBufferDescriptor){
                    .size = ssbo->capacity,
                    .mappedAtCreation = false,
                    .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage,
                    .label = ssbo_type[i].label,
                });
  }
}

SSBOStatus ssbo_update_entry(SSBOManager *manager, const SSBOType type,
                             size_t index, void *data) {

  if (index >= SSBO_CAPACITY) {
    VERBOSE_WARNING(
        "Attempting to write into SSBO out of bound index (%lu) max SSBO "
        "capacity is currently set to %d.",
        index, SSBO_CAPACITY);
    return SSBOStatus_OutOfBound;
  }

  SSBO *ssbo = &manager->buffers[type];
  memcpy((char *)ssbo->entries + index * ssbo->type_size, data,
         ssbo->type_size);

  // TODO improve index incrementation (currently very unsafe)
  manager->buffers[type].length = index + 1;

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
                         (uint8_t *)ssbo->entries + offset, ssbo->type_size);
  }

  return stagging_udpate;
}

void ssbo_upload(SSBOManager *manager, const SSBOType type) {

  SSBO *ssbo = &manager->buffers[type];
  wgpuQueueWriteBuffer(manager->queue, ssbo->buffer, 0, ssbo->entries,
                       ssbo->type_size * SSBO_CAPACITY);
}

WGPUBuffer ssbo_buffer(SSBOManager *manager, const SSBOType type) {
  return manager->buffers[type].buffer;
}

void *ssbo_new_entry(SSBOManager *manager, const SSBOType type, size_t *index) {
  SSBO *ssbo = &manager->buffers[type];
  return stli_new_entry((void *)&ssbo->entries, ssbo->capacity, &ssbo->length,
                        ssbo->type_size, "SSBO Manager");
}

StaticListStatus ssbo_remove_entry(SSBOManager *manager, const SSBOType type,
                                   size_t index) {
  SSBO *ssbo = &manager->buffers[type];
  return stli_remove((void *)&ssbo->entries, &ssbo->length, ssbo->type_size,
                     &ssbo->entries[index], "SSBO Manager");
}

size_t ssbo_length(SSBOManager *ssbo, const SSBOType type) {
  return ssbo->buffers[type].length;
}

void *ssbo_entry(SSBOManager *ssbo, const SSBOType type, size_t index) {
  return (void *)((uint8_t *)ssbo->buffers[type].entries +
                  index * ssbo->buffers[type].type_size);
}
