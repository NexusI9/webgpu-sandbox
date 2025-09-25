#ifndef _SHADER_STORAGE_BUFFER_OBJECT_H_
#define _SHADER_STORAGE_BUFFER_OBJECT_H_

#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <webgpu/webgpu.h>
#include <stdint.h>

#include "utils/stli.h"

#define SSBO_TYPE_COUNT 10
#define SSBO_CAPACITY 128
#define SSBO_MAX_TYPE_SIZE 2048
#define SSBO_UPDATE_QUEUE_CAPACITY 128

#define SSBO_INDEX_UNFOUND UINT32_MAX

/**

   SSBO and UBO managers both offer centralized interfaces for memory
   management. Those entities have fields from which their respective buffers
   are shared amongst many meshes (ex: Camera/ Viewport matrix).

   Such approach allow to only update 1 shared buffer rather that N independant
   buffer (hosted by N meshes) which drastically improved performances.

   SSBO offer either the possibility to directly write in GPU or to Queue a
   specific index (from a given field).

   Note regarding the Shader PSO:
   While UBO relies on Uniform, SSBO relies on Storage coupled with a dynamic
   offset. Storage are specific buffer type defined in the PSO:

    .--- Shader PSO --------------------------------------------------------.
    |       ...                                                             |
    |           (WGPUBufferBindingLayout){                                  |
    |               .type = WGPUBufferBindingType_ReadOnlyStorage,          |
    |               .hasDynamicOffset = false,                              |
    |               .minBindingSize = sizeof(SpotLightUniform),             |
    |            }                                                          |
    |       ...                                                             |
    '-----------------------------------------------------------------------'

    For storage the 'minBindSize' is important as it acts as an range of data
    available in the shader.

    Meaning in the case above, only 1 SpotLightUniform entry will be available
   (i.e. 1 x sizeof(SpotLightUniform))

    If we use an offset and only plan to read from 1 item (at index 0) this is
    fine. However if we plan to read multiple entry from the storage, we need to
    define the amplitude of available elements, which would give:

    .--- Shader PSO --------------------------------------------------------.
    |      ...                                                              |
    |           (WGPUBufferBindingLayout){                                  |
    |               .type = WGPUBufferBindingType_ReadOnlyStorage,          |
    |               .hasDynamicOffset = false,                              |
    |               .minBindingSize = sizeof(SpotLightUniform) * amplitude, |
    |            }                                                          |
    |       ...                                                             |
    '-----------------------------------------------------------------------'

    It's also worth noting that WebGPU seems to clamp the storage indexing,
   meaning that in case we try to reach an index out of bound, WebGPU will clamp
   the index to the latest index accesible.

 */

typedef size_t ssbo_id_t;

typedef enum {
  SSBOStatus_Success,
  SSBOStatus_OutOfBound,
  SSBOStatus_UndefError,
} SSBOStatus;

typedef enum {
  SSBOType_Camera,
  SSBOType_Viewport,
  SSBOType_Mesh,
  SSBOType_AmbientLight,
  SSBOType_PointLight,
  SSBOType_SunLight,
  SSBOType_SpotLight,
  SSBOType_ProbeGridReflection,
  SSBOType_ProbePlaneReflection,
  SSBOType_ViewProjection,
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

} SSBOManager;

void ssbo_draw_callback(void *);

void ssbo_init(SSBOManager *);
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

/* ==== SLOT ====*/

static inline void ssbo_slot_init_alloc(SSBOSlot *slot, size_t type_size) {
  slot->uniform = malloc(type_size);
  memset(slot->uniform, 0, type_size);
  slot->id = SSBO_INDEX_UNFOUND;
}

static inline void ssbo_slot_set_uniform(SSBOSlot *slot, const void *data,
                                         const size_t type_size) {
  memcpy(slot->uniform, data, type_size);
}

/**
   Transfers the given SSBOSlot to the manager
 */
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
