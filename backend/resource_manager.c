#include "./resource_manager.h"
#include "backend/context.h"
#include "backend/logger.h"
#include "backend/registry.h"
#include "backend/std_texture/core.h"
#include "runtime/light/core.h"
#include "runtime/mesh/core.h"
#include "runtime/probe/reflection/plane.h"
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

//---------------------------------------------//
// TEXTURE
//---------------------------------------------//
WGPUTexture rem_new_texture(const WGPUTextureDescriptor *desc) {

  const REMType type = REMType_Texture;

  // first generate hash source
  WGPUTexture texture = wgpuDeviceCreateTexture(context_device(), desc);

  REMTexture *entry = hsht_new_entry(&g_rem.entries[type], texture,
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

//---------------------------------------------//
// VIEW
//---------------------------------------------//
WGPUTextureView rem_new_view(const WGPUTexture texture,
                             const WGPUTextureViewDescriptor *desc) {

  const REMType type = REMType_View;

  WGPUTextureView view = wgpuTextureCreateView(texture, desc);

  REMView *entry = hsht_new_entry(&g_rem.entries[type], view,
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

//---------------------------------------------//
// SAMPLER
//---------------------------------------------//
WGPUSampler rem_new_sampler(const WGPUSamplerDescriptor *desc) {

  const REMType type = REMType_Sampler;

  WGPUSampler sampler = wgpuDeviceCreateSampler(context_device(), desc);

  REMSampler *entry = hsht_new_entry(&g_rem.entries[type], sampler,
                                     HashTableNewFlag_FixedCapacity);

  if (entry == NULL)
    return NULL;

  entry->owner = 0;
  entry->key = 0;
  entry->type = type;
  entry->handle = sampler;

  return sampler;
}

//---------------------------------------------//
// BUFFER
//---------------------------------------------//
WGPUBuffer rem_new_buffer(const WGPUBufferDescriptor *desc) {

  const REMType type = REMType_Buffer;

  WGPUBuffer buffer = wgpuDeviceCreateBuffer(context_device(), desc);

  REMBuffer *entry = hsht_new_entry(&g_rem.entries[type], buffer,
                                    HashTableNewFlag_FixedCapacity);

  if (entry == NULL) {
    rem_destroy_buffer(&buffer);
    return NULL;
  }

  entry->owner = 0;
  entry->key = 0;
  entry->type = type;
  entry->handle = buffer;

  return buffer;
}

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

//---------------------------------------------//
// SHADER MODULE
//---------------------------------------------//
WGPUShaderModule rem_new_shader_module(char *code, const char *label,
                                       const REMWriteFlag flag) {

  const REMType type = REMType_ShaderModule;

  WGPUShaderModuleWGSLDescriptor wgsl = {
      .chain.sType = WGPUSType_ShaderModuleWGSLDescriptor,
      .code = code,
  };

  WGPUShaderModule shader = wgpuDeviceCreateShaderModule(
      context_device(), &(WGPUShaderModuleDescriptor){
                            .nextInChain = (WGPUChainedStruct *)(&wgsl),
                            .label = label,
                        });

  REMShaderModule *entry = hsht_new_entry(&g_rem.entries[type], shader,
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

/*


   ▗▄▄▄▖▗▖  ▗▖ ▗▄▄▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖
   ▐▌   ▐▛▚▖▐▌▐▌     █  ▐▛▚▖▐▌▐▌
   ▐▛▀▀▘▐▌ ▝▜▌▐▌▝▜▌  █  ▐▌ ▝▜▌▐▛▀▀▘
   ▐▙▄▄▖▐▌  ▐▌▝▚▄▞▘▗▄█▄▖▐▌  ▐▌▐▙▄▄▖


 */

//---------------------------------------------//
// BASE
//---------------------------------------------//
#define REM_NEW_ENGINE_ITEM(Name, FuncName, ListName, REMItem, TypeEnum,       \
                            RegisterType)                                      \
  Name *rem_new_##FuncName() {                                                 \
                                                                               \
    const reg_id_t id = reg_new_id();                                          \
    if (id == REG_MAX_OBJECTS)                                                 \
      return NULL;                                                             \
                                                                               \
    REMItem *entry = hsht_new_entry(&g_rem.entries[TypeEnum], (void *)&id,     \
                                    HashTableNewFlag_FixedCapacity);           \
                                                                               \
    if (entry == NULL)                                                         \
      return NULL;                                                             \
                                                                               \
    reg_register(id, &entry->handle, RegisterType);                            \
    entry->handle.id = id;                                                     \
                                                                               \
    entry->owner = 0;                                                          \
    entry->key = 0;                                                            \
    entry->type = TypeEnum;                                                    \
    return &entry->handle;                                                     \
  }

REM_NEW_ENGINE_ITEM(Mesh, mesh, meshes, REMMesh, REMType_Mesh,
                    RegEntryType_Mesh);
REM_NEW_ENGINE_ITEM(Scene, scene, scenes, REMScene, REMType_Scene,
                    RegEntryType_Scene);

REM_NEW_ENGINE_ITEM(Shader, shader, shaders, REMShader, REMType_Shader,
                    RegEntryType_Shader);

// === Lights ===
REM_NEW_ENGINE_ITEM(PointLight, point_light, point_lights, REMPointLight,
                    REMType_PointLight, RegEntryType_PointLight);

REM_NEW_ENGINE_ITEM(AmbientLight, ambient_light, ambient_lights,
                    REMAmbientLight, REMType_AmbientLight,
                    RegEntryType_AmbientLight);

REM_NEW_ENGINE_ITEM(SpotLight, spot_light, spot_lights, REMSpotLight,
                    REMType_SpotLight, RegEntryType_SpotLight);

REM_NEW_ENGINE_ITEM(SunLight, sun_light, sun_lights, REMSunLight,
                    REMType_SunLight, RegEntryType_SunLight);

// === Probe / Reflection ===
REM_NEW_ENGINE_ITEM(ProbeReflectionPlane, plane_reflection, plane_reflections,
                    REMPlaneReflection, REMType_PlaneReflection,
                    RegEntryType_ProbeReflectionPlane);

REM_NEW_ENGINE_ITEM(ProbeReflection, probe_reflection, probe_reflections,
                    REMProbeReflection, REMType_ProbeReflection,
                    RegEntryType_ProbeReflection);

REM_NEW_ENGINE_ITEM(ProbeReflectionGrid, probe_reflection_grid,
                    probe_reflection_grids, REMProbeReflectionGrid,
                    REMType_ProbeReflectionGrid,
                    RegEntryType_ProbeReflectionGrid);

// Destroy item based on its handle pointer (wgpu Opaque Pointer objects)
#define REM_DESTROY_OPAQUE_ITEM(FuncName, HandleType, REMType, REMItem,        \
                                Destructor)                                    \
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

REM_DESTROY_OPAQUE_ITEM(texture, WGPUTexture, REMType_Texture, REMTexture,
                        wgpuTextureRelease(*handle));

REM_DESTROY_OPAQUE_ITEM(view, WGPUTextureView, REMType_View, REMView, {
  if (is_std_texture_view(*handle))
    return REMStatus_ProtectedResource;

  wgpuTextureViewRelease(*handle);
});

REM_DESTROY_OPAQUE_ITEM(buffer, WGPUBuffer, REMType_Buffer, REMBuffer,
                        wgpuBufferRelease(*handle));

REM_DESTROY_OPAQUE_ITEM(sampler, WGPUSampler, REMType_Sampler, REMSampler,
                        wgpuSamplerRelease(*handle));

REM_DESTROY_OPAQUE_ITEM(shader_module, WGPUShaderModule, REMType_ShaderModule,
                        REMShaderModule, wgpuShaderModuleRelease(*handle));

// Destroy item based on its handle id (engine objects)
#define REM_DESTROY_ENGINE_ITEM(FuncName, HandleType, REMType, REMItem,        \
                                Destructor)                                    \
  REMStatus rem_destroy_##FuncName(HandleType *handle) {                       \
                                                                               \
    if (handle == NULL)                                                        \
      return REMStatus_NullResource;                                           \
                                                                               \
    Destructor(handle);                                                        \
                                                                               \
    hsht_remove_entry(&g_rem.entries[REMType], &handle->id);                   \
                                                                               \
    return REMStatus_Success;                                                  \
  }

REM_DESTROY_ENGINE_ITEM(shader, Shader, REMType_Shader, REMShader,
                        shader_destroy);

REM_DESTROY_ENGINE_ITEM(mesh, Mesh, REMType_Mesh, REMMesh, mesh_destroy);

REM_DESTROY_ENGINE_ITEM(scene, Scene, REMType_Scene, REMScene, scene_destroy);

REM_DESTROY_ENGINE_ITEM(point_light, PointLight, REMType_PointLight,
                        REMPointLight, point_light_destroy);

REM_DESTROY_ENGINE_ITEM(ambient_light, AmbientLight, REMType_AmbientLight,
                        REMAmbientLight, ambient_light_destroy);

REM_DESTROY_ENGINE_ITEM(spot_light, SpotLight, REMType_SpotLight, REMSpotLight,
                        spot_light_destroy);

REM_DESTROY_ENGINE_ITEM(sun_light, SunLight, REMType_SunLight, REMSunLight,
                        sun_light_destroy);

REM_DESTROY_ENGINE_ITEM(plane_reflection, ProbeReflectionPlane,
                        REMType_PlaneReflection, REMPlaneReflection,
                        probe_reflection_plane_destroy);

REM_DESTROY_ENGINE_ITEM(probe_reflection, ProbeReflection,
                        REMType_ProbeReflection, REMProbeReflection,
                        probe_reflection_destroy);

REM_DESTROY_ENGINE_ITEM(probe_reflection_grid, ProbeReflectionGrid,
                        REMType_ProbeReflectionGrid, REMProbeReflectionGrid,
                        probe_reflection_grid_destroy);

// TODO
REMStatus rem_destroy_mbin(const char *handle) { return REMStatus_Success; }
REMStatus rem_destroy_gltf(const char *handle) { return REMStatus_Success; }
