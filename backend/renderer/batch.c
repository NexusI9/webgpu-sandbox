#include "batch.h"
#include "backend/logger.h"
#include "backend/renderer/core.h"
#include "backend/std_pipeline/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/pipeline/render.h"
#include "utils/hsht.h"
#include <stdint.h>

static const RendererBatchKey renderer_batch_config[] = {

    // Unlit
    {
        "Skybox",
        RenderPipelineType_Skybox,
        RendererBatchFlag_Fixed,
        RendererBatchLayer_Default,
        RendererDrawMode_Texture,
    },
    {
        "Unlit",
        RenderPipelineType_Unlit_Stencil,
        RendererBatchFlag_Reflection,
        RendererBatchLayer_Default,
        RendererDrawMode_Texture,
    },
    {
        "Gizmo",
        RenderPipelineType_Unlit,
        RendererBatchFlag_Fixed,
        RendererBatchLayer_Gizmo,
        RendererDrawMode_All,
    },
    {
        "Outline",
        RenderPipelineType_Outline,
        RendererBatchFlag_Fixed | RendererBatchFlag_Selection,
        RendererBatchLayer_Outline,
        RendererDrawMode_All,
    },
    {
        "Stencil",
        RenderPipelineType_Default,
        RendererBatchFlag_Fixed | RendererBatchFlag_Selection,
        RendererBatchLayer_Default,
        RendererDrawMode_All,
    },
    {
        "Glass Probe Grid",
        RenderPipelineType_GlassProbeGrid,
        RendererBatchFlag_None,
        RendererBatchLayer_Default,
        RendererDrawMode_Texture,
    },
    {
        "Glass Probe Plane",
        RenderPipelineType_GlassProbePlane,
        RendererBatchFlag_None,
        RendererBatchLayer_Default,
        RendererDrawMode_Texture,
    },
    {
        "Reflection",
        RenderPipelineType_Reflection,
        RendererBatchFlag_None,
        RendererBatchLayer_Default,
        RendererDrawMode_None,
    },
    {
        "Default",
        RenderPipelineType_Default,
        RendererBatchFlag_Shadow | RendererBatchFlag_Reflection,
        RendererBatchLayer_Default,
        RendererDrawMode_Texture,
    },
    {
        "PBR",
        RenderPipelineType_PBR,
        RendererBatchFlag_Shadow | RendererBatchFlag_Reflection |
            RendererBatchFlag_Lit,
        RendererBatchLayer_Default,
        RendererDrawMode_Texture,
    },
    {
        "PBR Double Sided",
        RenderPipelineType_PBR_DoubleSided,
        RendererBatchFlag_Shadow | RendererBatchFlag_Reflection |
            RendererBatchFlag_Lit,
        RendererBatchLayer_Default,
        RendererDrawMode_Texture,
    },
    {
        "PBR Alpha",
        RenderPipelineType_PBR_Alpha,
        RendererBatchFlag_Shadow | RendererBatchFlag_Reflection |
            RendererBatchFlag_Lit,
        RendererBatchLayer_Default,
        RendererDrawMode_Texture,
    },
    {
        "Grid",
        RenderPipelineType_Grid,
        RendererBatchFlag_Fixed,
        RendererBatchLayer_Default,
        RendererDrawMode_All,
    },
    {
        "Line", // For SEM that use lines we will use another config with
                // a fixed flag, cause for "Line based SEM" we use to set the
                // texture shader as Line and use the wireframe topology as the
                // base.
        RenderPipelineType_Line,
        RendererBatchFlag_None,
        RendererBatchLayer_Default,
        RendererDrawMode_Wireframe | RendererDrawMode_Boundbox,
    },
    {
        "Screen",
        RenderPipelineType_Screen,
        RendererBatchFlag_Fixed,
        RendererBatchLayer_Default,
        RendererDrawMode_None,
    },
    {
        "Shadow",
        RenderPipelineType_Shadow,
        RendererBatchFlag_None,
        RendererBatchLayer_Default,
        RendererDrawMode_None,
    },
    {
        "Solid",
        RenderPipelineType_Solid,
        RendererBatchFlag_None,
        RendererBatchLayer_Default,
        RendererDrawMode_Solid,
    },
    {
        "Billboard",
        RenderPipelineType_Billboard,
        RendererBatchFlag_Fixed,
        RendererBatchLayer_Default,
        RendererDrawMode_All,
    },

};

static const size_t renderer_batch_config_length =
    sizeof(renderer_batch_config) / sizeof(renderer_batch_config[0]);

/**

  ▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▄▄▄▖
    █  ▐▛▚▖▐▌  █    █
    █  ▐▌ ▝▜▌  █    █
  ▗▄█▄▖▐▌  ▐▌▗▄█▄▖  █

  Initialise the renderer batch hash table based on the configuration above.
  We also initalise eatch Renderer Batch Bucket's mesh list as to append meshes
  pointer in it.

  Renderer Batch allows to efficiently group meshes based on their pipeline and
  properties such as:
  - allow shadow pass
  - allow reflection pass
  - being in absolute front or in backgound
  - using a fixed render mode

  According to those proerties (defined as flags) as well as targeted render
  pipeline type we define a hash.
 */
RendererBatchStatus renderer_batch_init(HashTable *table,
                                        const size_t capacity) {

  // init hash table
  if (hsht_create(table,
                  &(HashTableDescriptor){
                      .label = "Renderer Batch",
                      .capacity = capacity,
                      .comparator_callback = renderer_batch_compare,
                      .generator_callback = renderer_batch_generate_hash,
                      .get_key_callback = renderer_batch_get_key,
                      .get_occupied_callback = renderer_batch_get_occupied,
                      .set_occupied_callback = renderer_batch_set_occupied,
                      .type_size = sizeof(RendererBatchBucket),
                  }) != HashTableStatus_Success) {
    logger_add(LoggerFlag_Error, "Unable to create renderer batch.");
    return RendererBatchStatus_InitFail;
  }

  // pre-compute above configuration slots and init mesh ref lists
  for (uint8_t i = 0; i < renderer_batch_config_length; i++) {

    RendererBatchBucket *bucket =
        hsht_new_entry(table, &renderer_batch_config[i], HashTableNewFlag_None);

    if (!bucket) {
      logger_add(LoggerFlag_Error,
                 "Unable to create renderer batch bucket no. %lu batch.", i);
      continue;
    }

    mesh_ref_list_create(&bucket->meshes, MESH_REF_LIST_CAPACITY);
    bucket->label = renderer_batch_config[i].label;
    bucket->key = renderer_batch_config[i];
    bucket->occupied = true;
  }

  return RendererBatchStatus_Success;
}

/*

   ▗▖ ▗▖ ▗▄▖  ▗▄▄▖▗▖ ▗▖
   ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌ ▐▌
   ▐▛▀▜▌▐▛▀▜▌ ▝▀▚▖▐▛▀▜▌
   ▐▌ ▐▌▐▌ ▐▌▗▄▄▞▘▐▌ ▐▌

   ▗▖ ▗▖▗▄▄▄▖▗▄▄▄▖▗▖    ▗▄▄▖
   ▐▌ ▐▌  █    █  ▐▌   ▐▌
   ▐▌ ▐▌  █    █  ▐▌    ▝▀▚▖
   ▝▚▄▞▘  █  ▗▄█▄▖▐▙▄▄▖▗▄▄▞▘


 */

uint32_t renderer_batch_generate_hash(const void *ptr) {

  RendererBatchKey *key = (RendererBatchKey *)ptr;

  uint32_t hash = 2166136261u;

  // Mix enum values
  hash ^= (uint32_t)key->pipeline;
  hash *= 16777619u;
  hash ^= (uint32_t)key->flags;
  hash *= 16777619u;
  hash ^= (uint32_t)key->layer;
  hash *= 16777619u;

  // Mix label string if present
  if (key->label) {
    uint32_t label_hash = hsht_hash_fnv1a32(key->label, strlen(key->label));
    // Basic avalanche mix (Xorshift + prime multiply)
    hash ^= label_hash + 0x9e3779b9u + (hash << 6) + (hash >> 2);
  }

  return hash;
}

bool renderer_batch_get_occupied(const void *obj) {
  return (((RendererBatchBucket *)obj)->occupied);
}

void renderer_batch_set_occupied(const void *bucket, const bool state) {
  ((RendererBatchBucket *)bucket)->occupied = state;
}

bool renderer_batch_compare(const void *k, const void *data) {

  RendererBatchBucket *bucket = (RendererBatchBucket *)data;
  RendererBatchKey *key = (RendererBatchKey *)k;

  return key->flags == bucket->key.flags &&
         key->pipeline == bucket->key.pipeline &&
         key->layer == bucket->key.layer;
}

void *renderer_batch_get_key(const void *bucket) {
  return &((RendererBatchBucket *)bucket)->key;
}

/**

   ▗▄▄▄ ▗▄▄▄▖ ▗▄▄▖▗▄▄▖▗▄▄▄▖▗▄▖  ▗▄▄▖▗▖ ▗▖
   ▐▌  █  █  ▐▌   ▐▌ ▐▌ █ ▐▌ ▐▌▐▌   ▐▌ ▐▌
   ▐▌  █  █   ▝▀▚▖▐▛▀▘  █ ▐▛▀▜▌▐▌   ▐▛▀▜▌
   ▐▙▄▄▀▗▄█▄▖▗▄▄▞▘▐▌    █ ▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌

   Add mesh to the dynamic pipeline.
   Depending on the mesh current texture pipeline, it will either dispatch the
   mesh to the unlit or lit pipeline.
   Basically if mesh texture shader has PBR pipeline it goes to the lit, if it
   has Unlit pieline it goes to the unlit.

   To add a mesh to the fixed pipelines (ex: Gizmo, Scene Editor Objects), the
   scene_add_mesh_pipeline dedicated function shall be used.

   The below function is designed for "common usage", meaning on a daily basis,
   one will add dynamic assets to the scene, compared to the fixed elements
   which are only used by the editor itself.
 */

const RendererBatchKey *
renderer_batch_get_key_from_pipeline(const RenderPipelineType pipeline_type) {

  if (pipeline_type == RENDER_PIPELINE_UNDEFINED) {
    logger_add(LoggerFlag_Error,
               "Couldn't find any valid type for mesh pipeline.");
    return NULL;
  }

  for (uint8_t i = 0; i < renderer_batch_config_length; i++)
    if (renderer_batch_config[i].pipeline == pipeline_type)
      return &renderer_batch_config[i];

  return NULL;
}

const RendererBatchKey *
renderer_batch_get_key_from_descriptor(const RendererBatchKeyDescriptor *desc) {

  for (uint8_t i = 0; i < renderer_batch_config_length; i++)
    if (renderer_batch_config[i].flags == desc->flags &&
        renderer_batch_config[i].layer == desc->layer &&
        renderer_batch_config[i].pipeline == desc->pipeline)
      return &renderer_batch_config[i];

  return NULL;
}

/**
   Retrieve the mesh lists from all the keys that use the given pipeline type.
 */
RendererBatchStatus renderer_batch_get_mesh_list_from_pipeline(
    HashTable *table, const RenderPipelineType pipeline_type,
    RendererBatchMeshLists *result) {

  *result = (RendererBatchMeshLists){0};

  for (uint8_t i = 0; i < renderer_batch_config_length; i++) {

    const RendererBatchKey *config_key = &renderer_batch_config[i];
    RendererBatchBucket *bucket = hsht_find(table, config_key, NULL);

    if (bucket && config_key->pipeline == pipeline_type &&
        result->length < RENDER_BATCH_LIST_CAPACITY)
      result->entries[result->length++] = &bucket->meshes;
  }

  if (result->length == 0) {
    logger_add(LoggerFlag_Error,
               "Unable to locate Mesh List for pipeline '%s', make sure the "
               "Renderer Batch "
               "configuration is correct and match with an existing one.",
               std_render_pipeline_label(pipeline_type));
    return RendererBatchStatus_UnfoundBatch;
  }

  return RendererBatchStatus_Success;
}

MeshRefList *
renderer_batch_get_mesh_list_from_key(HashTable *table,
                                      const RendererBatchKey *key) {

  RendererBatchBucket *bucket = hsht_find(table, key, NULL);

  if (bucket == NULL) {
    logger_add(LoggerFlag_Error,
               "Couldn't retrieve the renderer batch bucket from the given "
               "key '%s'.",
               key->label);
    return NULL;
  }

  return &bucket->meshes;
}

RendererBatchStatus
renderer_batch_get_mesh_list_with_flags(HashTable *table,
                                        const RendererBatchFlag flag,
                                        RendererBatchMeshLists *result) {

  *result = (RendererBatchMeshLists){0};

  for (uint8_t i = 0; i < renderer_batch_config_length; i++) {

    const RendererBatchKey *config_key = &renderer_batch_config[i];
    RendererBatchBucket *bucket = hsht_find(table, config_key, NULL);

    if (bucket && (config_key->flags & flag) &&
        result->length < RENDER_BATCH_LIST_CAPACITY)
      result->entries[result->length++] = &bucket->meshes;
  }

  return RendererBatchStatus_Success;
}

RendererBatchStatus
renderer_batch_get_mesh_list_without_flags(HashTable *table,
                                           const RendererBatchFlag flag,
                                           RendererBatchMeshLists *result) {

  *result = (RendererBatchMeshLists){0};

  for (uint8_t i = 0; i < renderer_batch_config_length; i++) {

    const RendererBatchKey *config_key = &renderer_batch_config[i];
    RendererBatchBucket *bucket = hsht_find(table, config_key, NULL);

    if (bucket && (config_key->flags & flag) == 0 &&
        result->length < RENDER_BATCH_LIST_CAPACITY)
      result->entries[result->length++] = &bucket->meshes;
  }

  return RendererBatchStatus_Success;
}

RendererBatchStatus
renderer_batch_get_mesh_list_from_layer(HashTable *table,
                                        const RendererBatchLayer layer,
                                        RendererBatchMeshLists *result) {

  *result = (RendererBatchMeshLists){0};

  for (uint8_t i = 0; i < renderer_batch_config_length; i++) {

    const RendererBatchKey *config_key = &renderer_batch_config[i];
    RendererBatchBucket *bucket = hsht_find(table, config_key, NULL);

    if (bucket && (config_key->layer & layer) &&
        result->length < RENDER_BATCH_LIST_CAPACITY)
      result->entries[result->length++] = &bucket->meshes;
  }

  return RendererBatchStatus_Success;
}

RendererBatchStatus
renderer_batch_get_configuration_keys(RendererBatchKeyList *list) {

  if (renderer_batch_config_length > RENDER_BATCH_LIST_CAPACITY) {
    logger_add(
        LoggerFlag_Warning,
        "The current Renderer Batch configuration's length (%lu) overpass key "
        "list max allowed capacity (%u).",
        renderer_batch_config_length, RENDER_BATCH_LIST_CAPACITY);
    return RendererBatchStatus_OutOfBound;
  }

  list->length = renderer_batch_config_length;

  for (uint8_t i = 0; i < list->length; i++)
    list->entries[i] = &renderer_batch_config[i];

  return RendererBatchStatus_Success;
}
