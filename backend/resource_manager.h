#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include "backend/logger.h"
#include "backend/registry.h"
#include "backend/ubo.h"
#include "runtime/camera/core.h"
#include "runtime/gui/core.h"
#include "runtime/light/core.h"
#include "runtime/light/list.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/compute.h"
#include "runtime/pipeline/render.h"
#include "runtime/probe/reflection/grid.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/probe/reflection/probe.h"
#include "runtime/scene/core.h"
#include "runtime/scene/renderer/core.h"
#include "runtime/shader/core.h"
#include "runtime/texture/core.h"
#include "utils/defines.h"
#include "utils/hsht.h"
#include "webgpu/webgpu.h"
#include <stddef.h>

typedef enum {
  REMStatus_Success,
  REMStatus_MaxCapacity,
  REMStatus_AllocFail,
  REMStatus_NullResource,
  REMStatus_UnfoundResource,
  REMStatus_ProtectedResource,
  REMStatus_UndefError,
} REMStatus;

// WGPU

// clang-format off

//      Type         |             Registry type           |         Label          |            Hash method         |   Capacity
#define REM_WGPU_LIST(_)                                                                                                           \
    _(WGPUTexture,             RegEntryType_WGPUObject,             texture,                 rem_generate_ptr_hash,          127 ) \
    _(WGPUTextureView,         RegEntryType_WGPUObject,             view,                    rem_generate_ptr_hash,          683 ) \
    _(WGPUBuffer,              RegEntryType_WGPUObject,             buffer,                  rem_generate_ptr_hash,          683 ) \
    _(WGPUShaderModule,        RegEntryType_WGPUObject,             shader_module,           rem_generate_ptr_hash,          127 ) \
    _(WGPUSampler,             RegEntryType_WGPUObject,             sampler,                 rem_generate_ptr_hash,          127 )                        


#define REM_ENGINE_LIST(_)                                                                                                         \
    _(Mesh,                   RegEntryType_Mesh,                    mesh,                    rem_generate_id_hash,           127 ) \
    _(Scene,                  RegEntryType_Scene,                   scene,                   rem_generate_id_hash,             3 ) \
    _(RenderPipeline,         RegEntryType_RenderPipeline,          render_pipeline,         rem_generate_id_hash,            71 ) \
    _(ComputePipeline,        RegEntryType_ComputePipeline,         compute_pipeline,        rem_generate_id_hash,           127 ) \
    _(Shader,                 RegEntryType_Shader,                  shader,                  rem_generate_id_hash,           127 ) \
    _(Camera,                 RegEntryType_Camera,                  camera,                  rem_generate_id_hash,            31 ) \
    _(SceneRenderer,          RegEntryType_Renderer,                renderer,                rem_generate_id_hash,             3 ) \
    _(Gui,                    RegEntryType_Gui,                     gui,                     rem_generate_id_hash,             3 ) \
    _(UBOManager,             RegEntryType_Ubo,                     ubo,                     rem_generate_id_hash,             1 ) \
    _(PointLight,             RegEntryType_PointLight,              point_light,             rem_generate_id_hash,             7 ) \
    _(AmbientLight,           RegEntryType_AmbientLight,            ambient_light,           rem_generate_id_hash,            37 ) \
    _(SpotLight,              RegEntryType_SpotLight,               spot_light,              rem_generate_id_hash,            37 ) \
    _(SunLight,               RegEntryType_SunLight,                sun_light,               rem_generate_id_hash,            37 ) \
    _(ProbeReflectionPlane,   RegEntryType_ProbeReflectionPlane,    plane_reflection,        rem_generate_id_hash,            17 ) \
    _(ProbeReflection,        RegEntryType_ProbeReflection,         probe_reflection,        rem_generate_id_hash,            17 ) \
    _(ProbeReflectionGrid,    RegEntryType_ProbeReflectionGrid,     probe_reflection_grid,   rem_generate_id_hash,            17 )

// clang-format on

#define REM_TYPE_COUNT 21

// === Generates Type Enums ===
#define _(Type, RegistryType, Label, Hash, Capacity) REMType_##Type,
typedef enum { REM_WGPU_LIST(_) REM_ENGINE_LIST(_) } REMType;
#undef _

// === Generate Structs ===
#define REM_STRUCT_ITEM(Type, RegistryType, Label, Hash, Capacity)             \
  typedef struct {                                                             \
    reg_id_t owner;                                                            \
    uint32_t key;                                                              \
    REMType type;                                                              \
    bool occupied;                                                             \
    Type handle;                                                               \
  } REM##Type;

REM_WGPU_LIST(REM_STRUCT_ITEM);
REM_ENGINE_LIST(REM_STRUCT_ITEM);

// utils
typedef struct {
  reg_id_t owner;
  uint32_t key;
  REMType type;
  bool occupied;
  void *handle;
} REMVoid;

/* TODO:
Add the following entities ?
 - sem lists
 - sem

 make sure the remove their respective item->id = ....
 */
typedef struct ResourceManager {

  HashTable entries[REM_TYPE_COUNT];

} ResourceManager;

extern ResourceManager g_rem;

/*
  Hash utils callback

  In order to improve access to our resources O(1), we use a hash system.
  However we use two kinds of hash methods depending on the resource type:
  1. WGPU related resources used Opaque Pointers, as a result we use this
  pointer to generate the hash.
  2. Engine related resources (Mesh, Scene...) have an ID generated by the
  register. As a result we use this ID  to generate the hash.

  In the same way that both WGPU and Engine resources use their respective
  constructor (wgpuDeviceCreate.... vs item_create()) and destructor
  (wgpuResourceRelease() vs item_destroy())
 */
uint32_t rem_generate_ptr_hash(const void *);
uint32_t rem_generate_id_hash(const void *);
void rem_bucket_set_occupied(const void *, const bool);
bool rem_bucket_get_occupied(const void *);
bool rem_bucket_compare(const void *, const void *);

// === Define Config ===
static const struct {
  const char *label;
  const size_t capacity;
  const size_t type_size;
  hsht_hash_generator hash_generator;
} rem_config[] = {
#define _(Type, RegistryType, Label, Hash, Capacity)                           \
  [REMType_##Type] = {                                                         \
      .label = #Type,                                                          \
      .capacity = Capacity,                                                    \
      .type_size = sizeof(REM##Type),                                          \
      .hash_generator = Hash,                                                  \
  },
    REM_WGPU_LIST(_) REM_ENGINE_LIST(_)
#undef _
};

typedef enum {
  REMWriteFlag_None = 0,
  REMWriteFlag_FreeData = 1 << 0,
  REMWriteFlag_STBIFreeData = 1 << 1,
} REMWriteFlag;

EXTERN_C_BEGIN

REMStatus resource_manager_init();

WGPUTexture rem_new_texture(const WGPUTextureDescriptor *);

WGPUTextureView rem_new_view(const WGPUTexture,
                             const WGPUTextureViewDescriptor *);

WGPUBuffer rem_new_buffer(const WGPUBufferDescriptor *);

WGPUSampler rem_new_sampler(const WGPUSamplerDescriptor *);

WGPUShaderModule rem_new_shader_module(char *code, const char *label,
                                       const REMWriteFlag);

REMStatus rem_write_buffer(WGPUBuffer, const size_t, void *, const size_t,
                           const REMWriteFlag);
REMStatus rem_write_texture(WGPUTexture, void *, const size_t,
                            const TextureChannel, const size_t,
                            const REMWriteFlag);

REMStatus rem_destroy_texture(WGPUTexture *);
REMStatus rem_destroy_sampler(WGPUSampler *);
REMStatus rem_destroy_view(WGPUTextureView *);
REMStatus rem_destroy_buffer(WGPUBuffer *);
REMStatus rem_destroy_shader_module(WGPUShaderModule *);

// === Generate Engine creator / destructor functions
#define REM_ENGINE_FUNC_ITEM(Type, RegistryType, Label, Hash, Capacity)        \
  Type *rem_new_##Label();                                                     \
  REMStatus rem_destroy_##Label(Type *);

REM_ENGINE_LIST(REM_ENGINE_FUNC_ITEM);

EXTERN_C_END

#endif
