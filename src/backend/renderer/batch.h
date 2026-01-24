#ifndef _RENDERER_BATCH_H_
#define _RENDERER_BATCH_H_

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
  RendererBatchStatus_UnfoundBatch,
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
  RendererLayer_Default = 1 << 0,
  RendererLayer_Outline = 1 << 1,
  RendererLayer_Gizmo = 1 << 2,
  RendererLayer_All = ~0,
} RendererLayer;
#define RENDERER_LAYER_COUNT 3

typedef enum {
  RendererDrawMode_None = 0,
  RendererDrawMode_Boundbox = 1 << 0,
  RendererDrawMode_Wireframe = 1 << 1,
  RendererDrawMode_Solid = 1 << 2,
  RendererDrawMode_Texture = 1 << 3,
  RendererDrawMode_All = ~0,
} RendererDrawMode;
#define RENDERER_DRAW_MODE_COUNT 4

typedef struct {
  const char *label;
  RenderPipelineType pipeline;
  RendererBatchFlag flags;
  RendererLayer layer;
  RendererDrawMode draw_mode;
} RendererBatchKey;

typedef struct {
  RenderPipelineType pipeline;
  RendererBatchFlag flags;
  RendererLayer layer;
  RendererDrawMode draw_mode;
} RendererBatchKeyDescriptor;

typedef struct {
  HashTableBucketState state;
  const char *label;
  RendererBatchKey key;
  MeshRefList meshes;
} RendererBatchBucket;

// static lists used to retrieve renderer batch meshes with specific flags
#define RENDER_BATCH_LIST_CAPACITY 32
typedef struct {
  MeshRefList *entries[RENDER_BATCH_LIST_CAPACITY];
  size_t count;
} RendererBatchMeshLists;

typedef struct {
  const RendererBatchKey *entries[RENDER_BATCH_LIST_CAPACITY];
  size_t count;
} RendererBatchKeyList;

EXTERN_C_BEGIN

// === Constructor ===
RendererBatchStatus renderer_batch_init(HashTable *, const size_t);

// === Hash Utils ===
uint32_t renderer_batch_generate_hash(const void *);
void renderer_batch_set_bucket_state(const void *, const HashTableBucketState);
HashTableBucketState renderer_batch_get_bucket_state(const void *);
bool renderer_batch_compare(const void *, const void *);
void *renderer_batch_get_key(const void *);

// === Keys Accessors ===
const RendererBatchKey *
renderer_batch_get_key_from_pipeline(const RenderPipelineType);

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
renderer_batch_get_mesh_list_from_layer(HashTable *, const RendererLayer,
                                        RendererBatchMeshLists *);

EXTERN_C_END

#endif
