#ifndef _SHADER_UNIFORM_BUFFER_OBJECT_H_
#define _SHADER_UNIFORM_BUFFER_OBJECT_H_

#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <webgpu/webgpu.h>

#include "backend/registry.h"
#include "utils/defines.h"
#include "utils/stli.h"

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

#define UBO_CAPACITY 128
#define UBO_MAX_TYPE_SIZE 65536
#define UBO_UPDATE_QUEUE_CAPACITY 128
#define UBO_INDEX_UNFOUND UINT32_MAX

typedef size_t ubo_id_t;

typedef enum {
  UBOStatus_Success,
  UBOStatus_OutOfBound,
  UBOStatus_UndefError,
} UBOStatus;

// clang-format off
#define UBO_LIST(_)                                  \
  _(  Camera,            CameraUniform             ) \
  _(  Viewport,          ViewportUniform           ) \
  _(  Mesh,              MeshUniform               ) \
  _(  ViewProjection,    ProjectionUniform         ) \
  _(  LightList,         LightListUniform          ) \
  _(  ProbeList,         ProbeListUniform          ) \
  _(  Environment,       SceneEnvironmentUniform   )

// clang-format on
#define UBO_TYPE_COUNT 7

typedef enum {
#define _(Name, Uniform) UBOType_##Name,
  UBO_LIST(_)
#undef _
} UBOType;

typedef struct {
  ubo_id_t id;
  void *uniform;
} UBOSlot;

typedef struct {
  ubo_id_t entries[UBO_UPDATE_QUEUE_CAPACITY];
  size_t length;
  size_t capacity;
} UBOBufferUpdateQueue;

typedef struct {
  uint8_t entries[UBO_CAPACITY * UBO_MAX_TYPE_SIZE];
  UBOBufferUpdateQueue update_queue;
  size_t capacity;
  size_t length;
  WGPUBuffer handle;
  size_t type_size;
} __attribute__((aligned(16))) UBOBuffer;

typedef struct {

  reg_id_t id;
  UBOBuffer buffers[UBO_TYPE_COUNT];

} UBOManager;

EXTERN_C_BEGIN

void ubo_draw_callback(void *);

void ubo_init(UBOManager *);
void ubo_destroy(UBOManager *);
void ubo_upload(UBOManager *, const UBOType);

/* ==== GETTERS ==== */
WGPUBuffer ubo_buffer_handle(UBOManager *, const UBOType);
size_t ubo_length(UBOManager *, const UBOType);
size_t ubo_find_index(UBOManager *, const UBOType, void *);
void *ubo_entry(UBOManager *, const UBOType, ubo_id_t);

/* ==== SLOT MANAGEMENT ==== */
UBOStatus ubo_insert_entry(UBOManager *, const UBOType, UBOSlot *);
UBOStatus ubo_update_entry(UBOManager *, const UBOType, const UBOSlot *);
UBOStatus ubo_upload_entry(UBOManager *, const UBOType, const UBOSlot *);
StaticListStatus ubo_remove_entry(UBOManager *, const UBOType, ubo_id_t);
UBOSlot ubo_new_entry(UBOManager *, const UBOType);

/* ==== SLOT ====*/

static inline void ubo_slot_init_alloc(UBOSlot *slot, size_t type_size) {
  slot->uniform = malloc(type_size);
  memset(slot->uniform, 0, type_size);
  slot->id = UBO_INDEX_UNFOUND;
}

static inline void ubo_slot_set_uniform(UBOSlot *slot, const void *data,
                                        const size_t type_size) {
  memcpy(slot->uniform, data, type_size);
}

/**
   DELETEME ??
   Transfers the given UBOSlot to the manager
 */
static inline UBOStatus ubo_copy_entry(UBOManager *manager, const UBOType type,
                                       UBOSlot *slot) {

  UBOSlot new_slot = ubo_new_entry(manager, type);

  // update CPU Side with existing slot data
  ubo_update_entry(manager, type, slot);
  // write GPU Side
  ubo_upload_entry(manager, type, slot);

  // free old mesh uniform data
  free(slot->uniform);

  *slot = new_slot;

  return UBOStatus_Success;
}

static inline ubo_id_t ubo_slot_id(const UBOSlot *slot) { return slot->id; }

/* ==== UPDATE QUEUE ==== */
StaticListStatus ubo_update_queue_insert(UBOManager *, const UBOType,
                                         const ubo_id_t);
StaticListStatus ubo_update_queue_shift(UBOManager *, const UBOType);

EXTERN_C_END

#endif
