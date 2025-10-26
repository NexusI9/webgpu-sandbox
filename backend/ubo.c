#include "ubo.h"

#include <stdbool.h>
#include <stdint.h>

#include "backend/context.h"
#include "backend/logger.h"
#include "runtime/camera/core.h"
#include "runtime/light/list.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/core.h"
#include "runtime/probe/core.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/probe/reflection/probe.h"
#include "runtime/scene/environment/core.h"
#include "runtime/shader/utils.h"
#include "runtime/viewport/core.h"
#include "utils/projection.h"
#include "utils/stli.h"
#include "webgpu/webgpu.h"

static const struct {
  const size_t size;
  const char *label;
} ubo_type[UBO_TYPE_COUNT] = {
    [UBOType_Camera] =
        {
            sizeof(CameraUniform),
            "UBO Camera Buffer",
        },
    [UBOType_Viewport] =
        {
            sizeof(ViewportUniform),
            "UBO Viewport Buffer",
        },
    [UBOType_Mesh] =
        {
            sizeof(MeshUniform),
            "UBO Mesh Buffer",
        },
    [UBOType_ViewProjection] =
        {
            sizeof(ProjectionUniform),
            "UBO View Projection Buffer",
        },
    [UBOType_LightList] =
        {
            sizeof(LightListUniform),
            "Light List Uniform",
        },
    [UBOType_ProbeList] =
        {
            sizeof(ProbeListUniform),
            "Probe List Uniform",
        },
    [UBOType_Environment] =
        {
            sizeof(SceneEnvironmentUniform),
            "Environment Uniform",
        },
};

void ubo_init(UBOManager *manager) {

  logger_add(LoggerFlag_Process, "Initializing UBO Manager");

  const uint16_t alignment = 16;
  const uint16_t min_size = 256;

  for (UBOType i = 0; i < UBO_TYPE_COUNT; i++) {

    if (ubo_type[i].size % alignment != 0)
      logger_add(LoggerFlag_Warning,
                 "Attempting to set a UBO buffer (%d) not aligned with %hu "
                 "bytes (%lu). UBO Buffers require 256 alignment.",
                 i, alignment, ubo_type[i].size);

    if (ubo_type[i].size < min_size)
      logger_add(
          LoggerFlag_Warning,
          "Attempting to set a buffer (%d) not with a type size inferior to %hu"
          "bytes (%lu).",
          i, min_size, ubo_type[i].size);

    if (ubo_type[i].size > UBO_MAX_TYPE_SIZE)
      logger_add(LoggerFlag_Warning,
                 "Attempting to set a buffer (%d) with a type size "
                 "superior to the maximum allowed size (%hu)"
                 ", type size bytes %lu.",
                 i, UBO_MAX_TYPE_SIZE, ubo_type[i].size);

    UBOBuffer *ubo = &manager->buffers[i];
    ubo->type_size = ubo_type[i].size;
    ubo->length = 0;
    ubo->capacity = UBO_CAPACITY * UBO_MAX_TYPE_SIZE;
    ubo->update_queue.capacity = UBO_UPDATE_QUEUE_CAPACITY;
    ubo->handle = wgpuDeviceCreateBuffer(
        context_device(),
        &(WGPUBufferDescriptor){
            .size = ubo->capacity,
            .mappedAtCreation = false,
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
            .label = ubo_type[i].label,
        });
  }
}

UBOStatus ubo_update_entry(UBOManager *manager, const UBOType type,
                           const UBOSlot *slot) {

  if (slot->id >= UBO_CAPACITY) {
    logger_add(LoggerFlag_Warning,
               "Attempting to write into UBO out of bound index (%lu) max UBO "
               "capacity is currently set to %d.",
               slot->id, UBO_CAPACITY);
    return UBOStatus_OutOfBound;
  }

  UBOBuffer *ubo = &manager->buffers[type];
  memcpy((char *)ubo->entries + slot->id * ubo->type_size, slot->uniform,
         ubo->type_size);

  // TODO improve index incrementation (currently very unsafe)
  manager->buffers[type].length = slot->id + 1;

  return UBOStatus_Success;
}

UBOStatus ubo_upload_entry(UBOManager *manager, const UBOType type,
                           const UBOSlot *slot) {

  // update ubo buffer at index
  UBOBuffer *ubo = &manager->buffers[type];
  size_t offset = slot->id * ubo->type_size;

  wgpuQueueWriteBuffer(context_queue(), ubo->handle, offset,
                       (uint8_t *)ubo->entries + offset, ubo->type_size);

  return UBOStatus_Success;
}

void ubo_upload(UBOManager *manager, const UBOType type) {

  UBOBuffer *ubo = &manager->buffers[type];
  wgpuQueueWriteBuffer(context_queue(), ubo->handle, 0, ubo->entries,
                       ubo->type_size * UBO_CAPACITY);
}

WGPUBuffer ubo_buffer_handle(UBOManager *manager, const UBOType type) {
  return manager->buffers[type].handle;
}

UBOSlot ubo_new_entry(UBOManager *manager, const UBOType type) {
  UBOBuffer *ubo = &manager->buffers[type];

  return (UBOSlot){
      .uniform = stli_new_entry((void *)ubo->entries, ubo->capacity,
                                &ubo->length, ubo->type_size, "UBO Manager"),
      .id = ubo->length,
  };
}

StaticListStatus ubo_remove_entry(UBOManager *manager, const UBOType type,
                                  ubo_id_t index) {
  UBOBuffer *ubo = &manager->buffers[type];
  return stli_remove((void *)ubo->entries, &ubo->length, ubo->type_size,
                     &ubo->entries[index], "UBO Manager");
}

size_t ubo_length(UBOManager *ubo, const UBOType type) {
  return ubo->buffers[type].length;
}

void *ubo_entry(UBOManager *ubo, const UBOType type, ubo_id_t index) {
  return (void *)((uint8_t *)ubo->buffers[type].entries +
                  index * ubo->buffers[type].type_size);
}

size_t ubo_find_index(UBOManager *manager, const UBOType type, void *data) {
  for (size_t i = 0; i < ubo_length(manager, UBOType_Mesh); i++)
    if (ubo_entry(manager, UBOType_Mesh, i) == data)
      return i;

  return UBO_INDEX_UNFOUND;
}

/**
   Transfer the slot data into the UBO buffer and update the slot id.
 */
UBOStatus ubo_insert_entry(UBOManager *manager, const UBOType type,
                           UBOSlot *slot) {
  ubo_new_entry(manager, type);
  ubo_update_entry(manager, type, slot);
  return ubo_upload_entry(manager, type, slot);
}

/**
   Insert and id of a specific Buffer type (view, mesh...) into the buffer
   update queue. Each queue is then read during the ubo_draw to write at the
   corresponding index.
   Using a queue prevent having "bool" last minutes flags.
 */
StaticListStatus ubo_update_queue_insert(UBOManager *manager,
                                         const UBOType type,
                                         const ubo_id_t id) {

  UBOBufferUpdateQueue *queue = &manager->buffers[type].update_queue;

  return stli_insert((void *)queue->entries, queue->capacity, &queue->length,
                     sizeof(ubo_id_t), (void *)&id, "UBO Update Queue");
}

StaticListStatus ubo_update_queue_shift(UBOManager *manager,
                                        const UBOType type) {
  UBOBufferUpdateQueue *queue = &manager->buffers[type].update_queue;
  return stli_shift((void *)queue->entries, &queue->length, sizeof(ubo_id_t),
                    "UBO Update Queue");
}

void ubo_draw_callback(void *data) {

  UBOManager *manager = (UBOManager *)data;

  for (UBOType type = 0; type < UBO_TYPE_COUNT; type++) {
    UBOBufferUpdateQueue *queue = &manager->buffers[type].update_queue;

    while (queue->length > 0) {
      ubo_id_t id = queue->entries[0];

      ubo_upload_entry(manager, type,
                       &(UBOSlot){
                           .id = id,
                           .uniform = &manager->buffers[type].entries[id],
                       });
      ubo_update_queue_shift(manager, type);
    }
  }
}
