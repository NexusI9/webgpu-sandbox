#include "./resource_manager.h"
#include "backend/context.h"
#include "backend/logger.h"
#include "backend/registry.h"
#include "backend/std_texture/core.h"
#include "backend/ubo.h"
#include "runtime/gui/core.h"
#include "runtime/light/core.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/render.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/scene/renderer/core.h"
#include "runtime/shader/core.h"
#include "stb/stb_image.h"
#include "utils/dyli.h"
#include "utils/hsht.h"
#include "utils/stli.h"
#include "webgpu/webgpu.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

ResourceManager g_rem = {0};

static inline const size_t rem_type_size(const REMType type) {
  return rem_config[type].type_size;
}

static inline const char *rem_type_label(const REMType type) {
  return rem_config[type].label;
}

static inline const size_t rem_type_init_capacity(const REMType type) {
  return rem_config[type].capacity;
}

static inline const hsht_hash_generator rem_type_hash_gen(const REMType type) {
  return rem_config[type].hash_generator;
}

static inline const size_t rem_type_length(const REMType type) {
  return g_rem.entries[type].occupied_list.length;
}

REMStatus resource_manager_init() {

  logger_add(LoggerFlag_Process, "Allocating space for resources...");

  REMStatus status = REMStatus_Success;
  size_t total_bytes = 0;

  for (REMType i = 0; i < REM_TYPE_COUNT; i++) {
    if (hsht_create(&g_rem.entries[i],
                    &(HashTableDescriptor){
                        .type_size = rem_type_size(i),
                        .label = rem_type_label(i),
                        .capacity = rem_type_init_capacity(i),
                        .generator_callback = rem_type_hash_gen(i),
                        .comparator_callback = rem_bucket_compare,
                        .get_occupied_callback = rem_bucket_get_occupied,
                        .set_occupied_callback = rem_bucket_set_occupied,
                    }) == HashTableStatus_Success) {

      total_bytes += rem_config[i].type_size * rem_config[i].capacity;

      logger_add(
          LoggerFlag_Print, "- %s: %lu bytes (type size: %lu, capacity: %lu)",
          rem_config[i].label, rem_config[i].type_size * rem_config[i].capacity,
          rem_config[i].type_size, rem_config[i].capacity);

    } else {
      logger_add(LoggerFlag_Error,
                 "- %s: Couldn't allocate memory with a "
                 "capacity of %lu (%lu bytes).",
                 rem_config[i].label, rem_config[i].capacity,
                 rem_config[i].type_size * rem_config[i].capacity);
      status = REMStatus_AllocFail;
    }
  }

  // log result
  logger_add(LoggerFlag_Print, "Total allocation size: %lu bytes", total_bytes);
  return status;
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

uint32_t rem_generate_ptr_hash(const void *ptr) { return hsht_hash_ptr(ptr); }

uint32_t rem_generate_id_hash(const void *id) {
  return hsht_hash_id(*(reg_id_t *)id);
}

bool rem_bucket_get_occupied(const void *obj) {
  return (bool)(((REMVoid *)obj)->occupied);
}
void rem_bucket_set_occupied(const void *obj, const bool state) {
  ((REMVoid *)obj)->occupied = state;
}

bool rem_bucket_compare(const void *ptr, const void *obj) {
  return ptr == ((REMVoid *)obj)->handle;
}

/*



   ▗▖ ▗▖ ▗▄▄▖▗▄▄▖ ▗▖ ▗▖
   ▐▌ ▐▌▐▌   ▐▌ ▐▌▐▌ ▐▌
   ▐▌ ▐▌▐▌▝▜▌▐▛▀▘ ▐▌ ▐▌
   ▐▙█▟▌▝▚▄▞▘▐▌   ▝▚▄▞▘



 */

WGPUTexture rem_new_texture(const WGPUTextureDescriptor *desc) {

  const REMType type = REMType_WGPUTexture;

  // first generate hash source
  WGPUTexture texture = wgpuDeviceCreateTexture(context_device(), desc);

  REMWGPUTexture *entry = hsht_new_entry(&g_rem.entries[type], texture,
                                         HashTableNewFlag_FixedCapacity);

  if (entry == NULL) {
    rem_destroy_texture(&texture);
    return NULL;
  }

  entry->owner = 0;
  entry->key = 0;
  entry->type = type;
  entry->handle = texture;

  return texture;
}

REMStatus rem_write_texture(WGPUTexture texture, void *data, const size_t size,
                            const TextureChannel channels, const size_t layer,
                            const REMWriteFlag flag) {

  if (texture == NULL) {
    logger_add(LoggerFlag_Error, "Attempting to write in undefined texture.");
    return REMStatus_NullResource;
  }

  if (data == NULL) {
    logger_add(LoggerFlag_Error,
               "Attempting to write in NULL data in texture.");
    return REMStatus_NullResource;
  }

  int width = wgpuTextureGetWidth(texture);
  int height = wgpuTextureGetHeight(texture);

  wgpuQueueWriteTexture(context_queue(),
                        &(WGPUImageCopyTexture){
                            .texture = texture,
                            .mipLevel = 0,
                            .origin = {0, 0, layer},
                            .aspect = WGPUTextureAspect_All,
                        },
                        data, size,
                        &(WGPUTextureDataLayout){
                            .offset = 0,
                            .bytesPerRow = width * channels,
                            .rowsPerImage = height,
                        },
                        &(WGPUExtent3D){width, height, 1});

  if (flag & REMWriteFlag_FreeData) {
    free(data);
    data = NULL;
  } else if (flag & REMWriteFlag_STBIFreeData) {
    stbi_image_free(data);
    data = NULL;
  }

  return REMStatus_Success;
}

#define REM_NEW_WGPU_ITEM(Name, FuncName, Type, REMName, DescType, Creator)    \
  Name rem_new_##FuncName(const DescType *desc) {                              \
                                                                               \
    const REMType type = Type;                                                 \
                                                                               \
    Name item = Creator(context_device(), desc);                               \
                                                                               \
    REMName *entry = hsht_new_entry(&g_rem.entries[type], item,                \
                                    HashTableNewFlag_FixedCapacity);           \
                                                                               \
    if (entry == NULL) {                                                       \
      rem_destroy_##FuncName(&item);                                           \
      return NULL;                                                             \
    }                                                                          \
                                                                               \
    entry->owner = 0;                                                          \
    entry->key = 0;                                                            \
    entry->type = type;                                                        \
    entry->handle = item;                                                      \
                                                                               \
    return item;                                                               \
  }

WGPUTextureView rem_new_view(const WGPUTexture texture,
                             const WGPUTextureViewDescriptor *desc) {

  const REMType type = REMType_WGPUTextureView;

  WGPUTextureView view = wgpuTextureCreateView(texture, desc);

  REMWGPUTextureView *entry = hsht_new_entry(&g_rem.entries[type], view,
                                             HashTableNewFlag_FixedCapacity);

  if (entry == NULL) {
    rem_destroy_view(&view);
    return NULL;
  }

  entry->owner = 0;
  entry->key = 0;
  entry->type = type;
  entry->handle = view;

  return view;
}

REM_NEW_WGPU_ITEM(WGPUSampler, sampler, REMType_WGPUSampler, REMWGPUSampler,
                  WGPUSamplerDescriptor, wgpuDeviceCreateSampler);

REM_NEW_WGPU_ITEM(WGPUBuffer, buffer, REMType_WGPUBuffer, REMWGPUBuffer,
                  WGPUBufferDescriptor, wgpuDeviceCreateBuffer);

REMStatus rem_write_buffer(WGPUBuffer buffer, const size_t offset, void *data,
                           const size_t size, const REMWriteFlag flag) {

  if (buffer == NULL) {
    logger_add(LoggerFlag_Error, "Attempting to write in undefined buffer.");
    return REMStatus_NullResource;
  }

  // if (data == NULL) {
  //   logger_add(LoggerFlag_Error, "Attempting to write in NULL data in
  //   buffer."); return REMStatus_NullResource;
  // }

  wgpuQueueWriteBuffer(context_queue(), buffer, offset, data, size);

  if (flag & REMWriteFlag_FreeData) {
    free(data);
    data = NULL;
  }

  return REMStatus_Success;
}

WGPUShaderModule rem_new_shader_module(char *code, const char *label,
                                       const REMWriteFlag flag) {

  const REMType type = REMType_WGPUShaderModule;

  WGPUShaderModuleWGSLDescriptor wgsl = {
      .chain.sType = WGPUSType_ShaderModuleWGSLDescriptor,
      .code = code,
  };

  WGPUShaderModule shader = wgpuDeviceCreateShaderModule(
      context_device(), &(WGPUShaderModuleDescriptor){
                            .nextInChain = (WGPUChainedStruct *)(&wgsl),
                            .label = label,
                        });

  REMWGPUShaderModule *entry = hsht_new_entry(&g_rem.entries[type], shader,
                                              HashTableNewFlag_FixedCapacity);

  if (entry == NULL) {
    rem_destroy_shader_module(&shader);
    return NULL;
  }

  if ((flag & REMWriteFlag_FreeData) && code) {
    free(code);
    code = NULL;
  }

  entry->owner = 0;
  entry->key = 0;
  entry->type = REMType_Shader;
  entry->handle = shader;

  return shader;
}

// Destroy item based on its handle pointer (wgpu Opaque Pointer objects)
#define REM_DESTROY_WGPU_ITEM(FuncName, HandleType, REMType, REMItem,          \
                              Destructor)                                      \
  REMStatus rem_destroy_##FuncName(HandleType *handle) {                       \
                                                                               \
    if (*handle == NULL)                                                       \
      return REMStatus_NullResource;                                           \
                                                                               \
    Destructor;                                                                \
                                                                               \
    hsht_remove_entry(&g_rem.entries[REMType], (void *)*handle);               \
                                                                               \
    *handle = NULL;                                                            \
                                                                               \
    return REMStatus_UnfoundResource;                                          \
  }

REM_DESTROY_WGPU_ITEM(texture, WGPUTexture, REMType_WGPUTexture, REMTexture,
                      wgpuTextureRelease(*handle));

REM_DESTROY_WGPU_ITEM(view, WGPUTextureView, REMType_WGPUTextureView, REMView, {
  if (is_std_texture_view(*handle))
    return REMStatus_ProtectedResource;

  wgpuTextureViewRelease(*handle);
});

REM_DESTROY_WGPU_ITEM(buffer, WGPUBuffer, REMType_WGPUBuffer, REMBuffer,
                      wgpuBufferRelease(*handle));

REM_DESTROY_WGPU_ITEM(sampler, WGPUSampler, REMType_WGPUSampler, REMSampler,
                      wgpuSamplerRelease(*handle));

REM_DESTROY_WGPU_ITEM(shader_module, WGPUShaderModule, REMType_WGPUShaderModule,
                      REMShaderModule, wgpuShaderModuleRelease(*handle));

/*


   ▗▄▄▄▖▗▖  ▗▖ ▗▄▄▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖
   ▐▌   ▐▛▚▖▐▌▐▌     █  ▐▛▚▖▐▌▐▌
   ▐▛▀▀▘▐▌ ▝▜▌▐▌▝▜▌  █  ▐▌ ▝▜▌▐▛▀▀▘
   ▐▙▄▄▖▐▌  ▐▌▝▚▄▞▘▗▄█▄▖▐▌  ▐▌▐▙▄▄▖


 */

#define REM_NEW_ENGINE_ITEM(Type, RegistryType, Label, Hash, Capacity)         \
  Type *rem_new_##Label() {                                                    \
                                                                               \
    const reg_id_t id = reg_new_id();                                          \
    if (id == REG_MAX_OBJECTS)                                                 \
      return NULL;                                                             \
                                                                               \
    REM##Type *entry =                                                         \
        hsht_new_entry(&g_rem.entries[REMType_##Type], (void *)&id,            \
                       HashTableNewFlag_FixedCapacity);                        \
                                                                               \
    if (entry == NULL)                                                         \
      return NULL;                                                             \
                                                                               \
    reg_register(id, &entry->handle, RegistryType);                            \
    entry->handle.id = id;                                                     \
                                                                               \
    entry->owner = 0;                                                          \
    entry->key = 0;                                                            \
    entry->type = REMType_##Type;                                              \
    return &entry->handle;                                                     \
  }

REM_ENGINE_LIST(REM_NEW_ENGINE_ITEM);

// Destroy item based on its handle id (engine objects)
#define REM_DESTROY_ENGINE_ITEM(Type, RegistryType, Label, Hash, Capacity)     \
  REMStatus rem_destroy_##Label(Type *handle) {                                \
                                                                               \
    if (handle == NULL)                                                        \
      return REMStatus_NullResource;                                           \
                                                                               \
    Label##_destroy(handle);                                                   \
                                                                               \
    hsht_remove_entry(&g_rem.entries[REMType_##Type], &handle->id);            \
                                                                               \
    return REMStatus_Success;                                                  \
  }

REM_ENGINE_LIST(REM_DESTROY_ENGINE_ITEM);
