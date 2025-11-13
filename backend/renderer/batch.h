#ifndef _RENDERER_BATCH_H_
#define _RENDERER_BATCH_H_

#include "backend/renderer/core.h"
#include "backend/std_pipeline/core.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/render.h"
#include "utils/defines.h"
#include "utils/hsht.h"

typedef enum {
  RendererBatchStatus_Success,
  RendererBatchStatus_AllocFail,
  RendererBatchStatus_OutOfBound,
  RendererBatchStatus_InitFail,
  RendererBatchStatus_UndefError,
} RendererBatchStatus;

typedef enum {
  RendererBatchFlag_None = 0,
  //  used for shadow map
  RendererBatchFlag_Shadow = 1 << 0,
  //  used for reflection pass
  RendererBatchFlag_Reflection = 1 << 1,
  //  keep the same shader (texture) no matter the rendering mode
  RendererBatchFlag_Fixed = 1 << 2,
  //  use alpha blend transparency and require dedicated sorting
  RendererBatchFlag_Alpha = 1 << 3,
  //  use light scene lights data (but not shadow map)
  RendererBatchFlag_Lit = 1 << 4,
  //  will host selected mesh (outline, stencil)
  RendererBatchFlag_Selection = 1 << 5,
} RendererBatchFlag;

typedef enum {
  RendererBatchLayer_Default,
  RendererBatchLayer_Outline,
  RendererBatchLayer_Gizmo,
} RendererBatchLayer;
#define RENDERER_BATCH_LAYER_COUNT 3

typedef struct {
  const char *label;
  RenderPipelineType pipeline;
  RendererBatchFlag flags;
  RendererBatchLayer layer;
  RendererDrawMode draw_mode;
} RendererBatchKey;

typedef struct {
  RenderPipelineType pipeline;
  RendererBatchFlag flags;
  RendererBatchLayer layer;
  RendererDrawMode draw_mode;
} RendererBatchKeyDescriptor;

typedef struct {
  bool occupied;
  const char *label;
  RendererBatchKey key;
  MeshRefList meshes;
} RendererBatchBucket;

// static lists used to retrieve renderer batch meshes with specific flags
#define RENDER_BATCH_LIST_CAPACITY 32
typedef struct {
  MeshRefList *entries[RENDER_BATCH_LIST_CAPACITY];
  size_t length;
} RendererBatchMeshLists;

typedef struct {
  const RendererBatchKey *entries[RENDER_BATCH_LIST_CAPACITY];
  size_t length;
} RendererBatchKeyList;

EXTERN_C_BEGIN

// === Constructor ===
RendererBatchStatus renderer_batch_init(HashTable *, const size_t);

// === Hash Utils ===
uint32_t renderer_batch_generate_hash(const void *);
void renderer_batch_set_occupied(const void *, const bool);
bool renderer_batch_get_occupied(const void *);
bool renderer_batch_compare(const void *, const void *);
void *renderer_batch_get_key(const void *);

// === Keys Accessors ===
const RendererBatchKey *
renderer_batch_get_key_from_pipeline(const RenderPipeline *);

const RendererBatchKey *
renderer_batch_get_key_from_descriptor(const RendererBatchKeyDescriptor *);

RendererBatchStatus
renderer_batch_get_configuration_keys(RendererBatchKeyList *);

// === Mesh List Accessors ===
RendererBatchStatus renderer_batch_get_mesh_list_from_pipeline(
    HashTable *, const RenderPipelineType, RendererBatchMeshLists *);

MeshRefList *renderer_batch_get_mesh_list_from_key(HashTable *,
                                                   const RendererBatchKey *);

RendererBatchStatus
renderer_batch_get_mesh_list_with_flags(HashTable *, const RendererBatchFlag,
                                        RendererBatchMeshLists *);

RendererBatchStatus
renderer_batch_get_mesh_list_without_flags(HashTable *, const RendererBatchFlag,
                                           RendererBatchMeshLists *);

RendererBatchStatus
renderer_batch_get_mesh_list_from_layer(HashTable *, const RendererBatchLayer,
                                        RendererBatchMeshLists *);

EXTERN_C_END

#endif
