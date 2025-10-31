#include "./resource_manager.h"
#include "backend/context.h"
#include "backend/logger.h"
#include "backend/std_texture/core.h"
#include "runtime/light/core.h"
#include "runtime/mesh/core.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/shader/core.h"
#include "utils/dyli.h"
#include "utils/stli.h"
#include "webgpu/webgpu.h"
#include <stdint.h>
#include <stdio.h>

ResourceManager g_rem = {0};

static inline const size_t rem_type_size(const REMType type) {
  return rem_config[type].type_size;
}

static inline const char *rem_type_label(const REMType type) {
  return rem_config[type].label;
}

static inline const size_t rem_type_length(const REMType type) {
  return g_rem.entries[type].length;
}

REMStatus resource_manager_init() {

  logger_add(LoggerFlag_Process, "Allocating space for resources...");

  REMStatus status = REMStatus_Success;
  size_t total_bytes = 0;

  for (REMType i = 0; i < REM_TYPE_COUNT; i++) {
    if (dyli_create(&g_rem.entries[i].entries, &g_rem.entries[i].capacity,
                    &g_rem.entries[i].length, rem_config[i].type_size,
                    rem_config[i].capacity,
                    rem_config[i].label) == DynamicListStatus_Success) {

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


   ▗▖ ▗▖▗▄▄▄▖▗▄▄▄▖▗▖    ▗▄▄▖
   ▐▌ ▐▌  █    █  ▐▌   ▐▌
   ▐▌ ▐▌  █    █  ▐▌    ▝▀▚▖
   ▝▚▄▞▘  █  ▗▄█▄▖▐▙▄▄▖▗▄▄▞▘



 */

static inline uint32_t rem_hash_key(const char *key) {
  if (!key)
    return 0;
  uint32_t hash = 2166136261u;
  for (; *key; key++)
    hash = (hash ^ (uint8_t)(*key)) * 16777619u;
  return hash;
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
  REMTexture *entry = dyli_new_entry(
      (void *)&g_rem.entries[type].entries, &g_rem.entries[type].capacity,
      &g_rem.entries[type].length, rem_type_size(type), rem_type_label(type));

  if (entry == NULL)
    return NULL;

  WGPUTexture texture = wgpuDeviceCreateTexture(context_device(), desc);

  size_t idx = rem_type_length(type) - 1;
  entry->owner = 0;
  entry->key = 0; // key ? rem_hash_key(key) : 0;
  entry->type = REMType_Texture;
  entry->handle = texture;

  return texture;
}

//---------------------------------------------//
// VIEW
//---------------------------------------------//
WGPUTextureView rem_new_view(const WGPUTexture texture,
                             const WGPUTextureViewDescriptor *desc) {

  const REMType type = REMType_View;
  REMView *entry = dyli_new_entry(
      (void *)&g_rem.entries[type].entries, &g_rem.entries[type].capacity,
      &g_rem.entries[type].length, rem_type_size(type), rem_type_label(type));

  if (entry == NULL)
    return NULL;

  WGPUTextureView view = wgpuTextureCreateView(texture, desc);

  size_t idx = rem_type_length(type) - 1;
  entry->owner = 0;
  entry->key = 0; // key ? rem_hash_key(key) : 0;
  entry->type = REMType_View;
  entry->handle = view;

  return view;
}

//---------------------------------------------//
// BUFFER
//---------------------------------------------//
WGPUBuffer rem_new_buffer(const WGPUBufferDescriptor *desc) {

  const REMType type = REMType_Buffer;
  REMBuffer *entry = dyli_new_entry(
      (void *)&g_rem.entries[type].entries, &g_rem.entries[type].capacity,
      &g_rem.entries[type].length, rem_type_size(type), rem_type_label(type));

  if (entry == NULL)
    return NULL;

  WGPUBuffer buffer = wgpuDeviceCreateBuffer(context_device(), desc);

  size_t idx = rem_type_length(type) - 1;
  entry->owner = 0;
  entry->key = 0; // key ? rem_hash_key(key) : 0;
  entry->type = REMType_Buffer;
  entry->handle = buffer;

  return buffer;
}

//---------------------------------------------//
// SHADER MODULE
//---------------------------------------------//
WGPUShaderModule rem_new_shader_module(const WGPUShaderModuleDescriptor *desc) {

  const REMType type = REMType_ShaderModule;
  REMShaderModule *entry = dyli_new_entry(
      (void *)&g_rem.entries[type].entries, &g_rem.entries[type].capacity,
      &g_rem.entries[type].length, rem_type_size(type), rem_type_label(type));

  if (entry == NULL)
    return NULL;

  WGPUShaderModule shader =
      wgpuDeviceCreateShaderModule(context_device(), desc);

  size_t idx = rem_type_length(type) - 1;
  entry->owner = 0;
  entry->key = 0; // key ? rem_hash_key(key) : 0;
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
#define REM_NEW_ENGINE_ITEM(Name, FuncName, ListName, REMItem, TypeEnum)       \
  Name *rem_new_##FuncName() {                                                 \
    REMItem *entry = dyli_new_entry(                                           \
        (void *)&g_rem.entries[TypeEnum].entries,                              \
        &g_rem.entries[TypeEnum].capacity, &g_rem.entries[TypeEnum].length,    \
        rem_type_size(TypeEnum), rem_type_label(TypeEnum));                    \
                                                                               \
    if (entry == NULL)                                                         \
      return NULL;                                                             \
                                                                               \
    size_t idx = rem_type_length(TypeEnum) - 1;                                \
    entry->owner = 0;                                                          \
    entry->key = 0;                                                            \
    entry->type = TypeEnum;                                                    \
    return &entry->handle;                                                     \
  }

REM_NEW_ENGINE_ITEM(Mesh, mesh, meshes, REMMesh, REMType_Mesh);
REM_NEW_ENGINE_ITEM(Scene, scene, scenes, REMScene, REMType_Scene);
REM_NEW_ENGINE_ITEM(Shader, shader, shaders, REMShader, REMType_Shader);

// === Lights ===
REM_NEW_ENGINE_ITEM(PointLight, point_light, point_lights, REMPointLight,
                    REMType_PointLight);

REM_NEW_ENGINE_ITEM(AmbientLight, ambient_light, ambient_lights,
                    REMAmbientLight, REMType_AmbientLight);

REM_NEW_ENGINE_ITEM(SpotLight, spot_light, spot_lights, REMSpotLight,
                    REMType_SpotLight);

REM_NEW_ENGINE_ITEM(SunLight, sun_light, sun_lights, REMSunLight,
                    REMType_SunLight);

// === Probe / Reflection ===
REM_NEW_ENGINE_ITEM(ProbeReflectionPlane, plane_reflection, plane_reflections,
                    REMPlaneReflection, REMType_PlaneReflection);

REM_NEW_ENGINE_ITEM(ProbeReflection, probe_reflection, probe_reflections,
                    REMProbeReflection, REMType_ProbeReflection);

REM_NEW_ENGINE_ITEM(ProbeReflectionGrid, probe_reflection_grid,
                    probe_reflection_grids, REMProbeReflectionGrid,
                    REMType_ProbeReflectionGrid);

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
    for (size_t i = 0; i < rem_type_length(REMType); i++) {                    \
                                                                               \
      REMItem *entry = (REMItem *)&g_rem.entries[REMType].entries[i];          \
                                                                               \
      if (entry->handle == *handle) {                                          \
        stli_remove_at_index(g_rem.entries[REMType].entries,                   \
                             &g_rem.entries[REMType].length,                   \
                             rem_type_size(REMType), i);                       \
        return REMStatus_Success;                                              \
      }                                                                        \
    }                                                                          \
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
    for (size_t i = 0; i < rem_type_length(REMType); i++) {                    \
                                                                               \
      REMItem *entry = (REMItem *)&g_rem.entries[REMType].entries[i];          \
                                                                               \
      if (entry->handle.id == handle->id) {                                    \
        stli_remove_at_index(g_rem.entries[REMType].entries,                   \
                             &g_rem.entries[REMType].length,                   \
                             rem_type_size(REMType), i);                       \
        return REMStatus_Success;                                              \
      }                                                                        \
    }                                                                          \
                                                                               \
    return REMStatus_UnfoundResource;                                          \
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
