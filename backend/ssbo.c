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
    [SSBOType_AmbientLight] =
        {
            sizeof(AmbientLightUniform),
            "SSBO Ambient Light Buffer",
        },
    [SSBOType_PointLight] =
        {
            sizeof(PointLightUniform),
            "SSBO Point Light Buffer",
        },
    [SSBOType_SunLight] =
        {
            sizeof(SunLightUniform),
            "SSBO Sun Light Buffer",
        },
    [SSBOType_SpotLight] =
        {
            sizeof(SpotLightUniform),
            "SSBO Spot Light Buffer",
        },
    [SSBOType_ViewShadow] =
        {
            sizeof(ProjectionUniform),
            "SSBO View Shadow Buffer",
        },
    [SSBOType_ViewProbeReflection] =
        {
            sizeof(ProjectionUniform),
            "SSBO View Probe Reflection Buffer",
        },
};

void ssbo_init(SSBOManager *manager, WGPUDevice device, WGPUQueue queue) {

  VERBOSE_PROCESS("Initializing SSBO Manager");

  manager->device = device;
  manager->queue = queue;

  for (SSBOType i = 0; i < SSBO_TYPE_COUNT; i++) {

    if (ssbo_type[i].size % 256 != 0)
      VERBOSE_WARNING(
          "Attempting to set a SSBO buffer (%d) not aligned with 256 "
          "bytes (%lu). SSBO Buffers require 256 alignment.",
          i, ssbo_type[i].size);

    SSBOBuffer *ssbo = &manager->buffers[i];
    ssbo->type_size = ssbo_type[i].size;
    ssbo->length = 0;
    ssbo->capacity = SSBO_CAPACITY * SSBO_MAX_TYPE_SIZE;
    ssbo->update_queue.capacity = SSBO_UPDATE_QUEUE_CAPACITY;
    ssbo->handle = wgpuDeviceCreateBuffer(
        device, &(WGPUBufferDescriptor){
                    .size = ssbo->capacity,
                    .mappedAtCreation = false,
                    .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage,
                    .label = ssbo_type[i].label,
                });
  }
}

SSBOStatus ssbo_update_entry(SSBOManager *manager, const SSBOType type,
                             const SSBOSlot *slot) {

  if (slot->id >= SSBO_CAPACITY) {
    VERBOSE_WARNING(
        "Attempting to write into SSBO out of bound index (%lu) max SSBO "
        "capacity is currently set to %d.",
        slot->id, SSBO_CAPACITY);
    return SSBOStatus_OutOfBound;
  }

  SSBOBuffer *ssbo = &manager->buffers[type];
  memcpy((char *)ssbo->entries + slot->id * ssbo->type_size, slot->uniform,
         ssbo->type_size);

  // TODO improve index incrementation (currently very unsafe)
  manager->buffers[type].length = slot->id + 1;

  return SSBOStatus_Success;
}

SSBOStatus ssbo_upload_entry(SSBOManager *manager, const SSBOType type,
                             const SSBOSlot *slot) {

  // update ssbo buffer at index
  SSBOBuffer *ssbo = &manager->buffers[type];
  size_t offset = slot->id * ssbo->type_size;
  wgpuQueueWriteBuffer(manager->queue, ssbo->handle, offset,
                       (uint8_t *)ssbo->entries + offset, ssbo->type_size);

  return SSBOStatus_Success;
}

void ssbo_upload(SSBOManager *manager, const SSBOType type) {

  SSBOBuffer *ssbo = &manager->buffers[type];
  wgpuQueueWriteBuffer(manager->queue, ssbo->handle, 0, ssbo->entries,
                       ssbo->type_size * SSBO_CAPACITY);
}

WGPUBuffer ssbo_buffer_handle(SSBOManager *manager, const SSBOType type) {
  return manager->buffers[type].handle;
}

void *ssbo_new_entry(SSBOManager *manager, const SSBOType type,
                     ssbo_id_t *index) {
  SSBOBuffer *ssbo = &manager->buffers[type];

  if (index)
    *index = ssbo->length;

  return stli_new_entry((void *)ssbo->entries, ssbo->capacity, &ssbo->length,
                        ssbo->type_size, "SSBO Manager");
}

StaticListStatus ssbo_remove_entry(SSBOManager *manager, const SSBOType type,
                                   ssbo_id_t index) {
  SSBOBuffer *ssbo = &manager->buffers[type];
  return stli_remove((void *)ssbo->entries, &ssbo->length, ssbo->type_size,
                     &ssbo->entries[index], "SSBO Manager");
}

size_t ssbo_length(SSBOManager *ssbo, const SSBOType type) {
  return ssbo->buffers[type].length;
}

void *ssbo_entry(SSBOManager *ssbo, const SSBOType type, ssbo_id_t index) {
  return (void *)((uint8_t *)ssbo->buffers[type].entries +
                  index * ssbo->buffers[type].type_size);
}

size_t ssbo_find_index(SSBOManager *manager, const SSBOType type, void *data) {
  for (size_t i = 0; i < ssbo_length(manager, SSBOType_Mesh); i++)
    if (ssbo_entry(manager, SSBOType_Mesh, i) == data)
      return i;

  return SSBO_INDEX_UNFOUND;
}

/**
   Transfer the slot data into the SSBO buffer and update the slot id.
 */
SSBOStatus ssbo_insert_entry(SSBOManager *manager, const SSBOType type,
                             SSBOSlot *slot) {

  ssbo_new_entry(manager, type, &slot->id);
  ssbo_update_entry(manager, type, slot);
  return ssbo_upload_entry(manager, type, slot);
}

/**
   Insert and id of a specific Buffer type (view, mesh...) into the buffer
   update queue. Each queue is then read during the ssbo_draw to write at the
   corresponding index.
   Using a queue prevent having "bool" last minutes flags.
 */
StaticListStatus ssbo_update_queue_insert(SSBOManager *manager,
                                          const SSBOType type,
                                          const ssbo_id_t id) {

  SSBOBufferUpdateQueue *queue = &manager->buffers[type].update_queue;
  return stli_insert((void *)queue->entries, queue->capacity, &queue->length,
                     sizeof(ssbo_id_t), (void *)&id, "SSBO Update Queue");
}

StaticListStatus ssbo_update_queue_shift(SSBOManager *manager,
                                         const SSBOType type) {
  SSBOBufferUpdateQueue *queue = &manager->buffers[type].update_queue;
  return stli_shift((void *)queue->entries, &queue->length, sizeof(ssbo_id_t),
                    "SSBO Update Queue");
}

void ssbo_draw_callback(void *data) {

  SSBOManager *manager = (SSBOManager *)data;

  for (SSBOType type = 0; type < SSBO_TYPE_COUNT; type++) {
    SSBOBufferUpdateQueue *queue = &manager->buffers[type].update_queue;

    while (queue->length > 0) {
      ssbo_id_t id = queue->entries[0];
      ssbo_upload_entry(manager, type,
                        &(SSBOSlot){
                            .id = id,
                            .uniform = &manager->buffers[type].entries[id],
                        });
      ssbo_update_queue_shift(manager, type);
    }
  }
}
