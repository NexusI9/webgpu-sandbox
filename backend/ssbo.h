#ifndef _SHADER_STORAGE_BUFFER_OBJECT_H_
#define _SHADER_STORAGE_BUFFER_OBJECT_H_

#include "../utils/stli.h"
#include <stdlib.h>
#include <string.h>
#include <webgpu/webgpu.h>

#define SSBO_TYPE_COUNT 7
#define SSBO_CAPACITY 32
#define SSBO_MAX_TYPE_SIZE 2048
#define SSBO_UPDATE_QUEUE_CAPACITY 64

#define SSBO_INDEX_UNFOUND UINT32_MAX

typedef size_t ssbo_id_t;

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
  WGPUBuffer handle;
  size_t type_size;
} SSBOBuffer;

typedef struct {
  ssbo_id_t id;
  void *uniform;
} SSBOSlot;

typedef struct {
  SSBOSlot entries[SSBO_UPDATE_QUEUE_CAPACITY];
  size_t length;
  size_t capacity;
} SSBOUpdateQueue;

typedef struct {
  SSBOBuffer buffers[SSBO_TYPE_COUNT];
  SSBOUpdateQueue update_queue;
  WGPUQueue queue;
  WGPUDevice device;
} SSBOManager;

SSBOStatus ssbo_update_queue_insert(SSBOManager *, const SSBOType,
                                    const SSBOSlot *);

SSBOStatus ssbo_update_queue_remove(SSBOManager *, const SSBOType,
                                    const SSBOSlot *);

void ssbo_init(SSBOManager *, WGPUDevice, WGPUQueue);
SSBOStatus ssbo_update_entry(SSBOManager *, const SSBOType, ssbo_id_t, void *);
SSBOStatus ssbo_upload_entry(SSBOManager *, const SSBOType, ssbo_id_t, void *);
void ssbo_upload(SSBOManager *, const SSBOType);

SSBOStatus ssbo_insert_slot(SSBOManager *, const SSBOType, SSBOSlot *);

void *ssbo_entry(SSBOManager *, const SSBOType, ssbo_id_t);
void *ssbo_new_entry(SSBOManager *, const SSBOType, ssbo_id_t *);
StaticListStatus ssbo_remove_entry(SSBOManager *, const SSBOType, ssbo_id_t);

WGPUBuffer ssbo_buffer_handle(SSBOManager *, const SSBOType);
size_t ssbo_length(SSBOManager *, const SSBOType);

size_t ssbo_find_index(SSBOManager *, const SSBOType, void *);

static inline SSBOStatus ssbo_transfer_data(SSBOManager *manager,
                                            const SSBOType type, void **src,
                                            void *dest) {
  if (src && dest) {
    memcpy(dest, *src, manager->buffers[type].type_size);
    free(*src);
    *src = dest;
    return SSBOStatus_Success;
  }

  return SSBOStatus_UndefError;
}

#endif
