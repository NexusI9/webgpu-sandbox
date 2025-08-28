#ifndef _SHADER_STORAGE_BUFFER_OBJECT_H_
#define _SHADER_STORAGE_BUFFER_OBJECT_H_

#include "../utils/stli.h"
#include <stdalign.h>
#include <stdio.h>
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
  ssbo_id_t id;
  void *uniform;
} SSBOSlot;

typedef struct {
  ssbo_id_t entries[SSBO_UPDATE_QUEUE_CAPACITY];
  size_t length;
  size_t capacity;
} SSBOBufferUpdateQueue;

typedef struct {
  uint8_t entries[SSBO_CAPACITY * SSBO_MAX_TYPE_SIZE];
  SSBOBufferUpdateQueue update_queue;
  size_t capacity;
  size_t length;
  WGPUBuffer handle;
  size_t type_size;
} __attribute__((aligned(16))) SSBOBuffer;

typedef struct {
  SSBOBuffer buffers[SSBO_TYPE_COUNT];
  WGPUQueue queue;
  WGPUDevice device;
} SSBOManager;

void ssbo_draw_callback(void *);

void ssbo_init(SSBOManager *, WGPUDevice, WGPUQueue);
void ssbo_upload(SSBOManager *, const SSBOType);

/* ==== GETTERS ==== */
WGPUBuffer ssbo_buffer_handle(SSBOManager *, const SSBOType);
size_t ssbo_length(SSBOManager *, const SSBOType);
size_t ssbo_find_index(SSBOManager *, const SSBOType, void *);
void *ssbo_entry(SSBOManager *, const SSBOType, ssbo_id_t);

/* ==== SLOT MANAGEMENT ==== */
SSBOStatus ssbo_insert_entry(SSBOManager *, const SSBOType, SSBOSlot *);
SSBOStatus ssbo_update_entry(SSBOManager *, const SSBOType, const SSBOSlot *);
SSBOStatus ssbo_upload_entry(SSBOManager *, const SSBOType, const SSBOSlot *);
StaticListStatus ssbo_remove_entry(SSBOManager *, const SSBOType, ssbo_id_t);
void *ssbo_new_entry(SSBOManager *, const SSBOType, ssbo_id_t *);

static inline SSBOStatus ssbo_copy_entry(SSBOManager *manager,
                                         const SSBOType type, SSBOSlot *slot) {

  size_t index;
  void *ssbo_uniform = ssbo_new_entry(manager, type, &index);
  slot->id = index;

  // update CPU Side with existing slot data
  ssbo_update_entry(manager, type, slot);
  // write GPU Side
  ssbo_upload_entry(manager, type, slot);

  // free old mesh uniform data
  free(slot->uniform);

  // assign ssbo pointer to mesh for future reference
  slot->uniform = ssbo_uniform;

  return SSBOStatus_Success;
}

/* ==== UPDATE QUEUE ==== */
StaticListStatus ssbo_update_queue_insert(SSBOManager *, const SSBOType,
                                          const ssbo_id_t);
StaticListStatus ssbo_update_queue_shift(SSBOManager *, const SSBOType);
#endif
