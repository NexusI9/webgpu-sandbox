#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include "backend/logger.h"
#include "backend/registry.h"
#include "runtime/light/core.h"
#include "runtime/light/list.h"
#include "runtime/mesh/core.h"
#include "runtime/probe/reflection/grid.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/probe/reflection/probe.h"
#include "runtime/scene/core.h"
#include "runtime/shader/core.h"
#include "webgpu/webgpu.h"

typedef enum {
  REMStatus_Success,
  REMStatus_MaxCapacity,
  REMStatus_AllocFail,
  REMStatus_NullResource,
  REMStatus_UnfoundResource,
  REMStatus_ProtectedResource,
  REMStatus_UndefError,
} REMStatus;

#define REM_TYPE_COUNT 16
typedef enum {
  REMType_Texture,
  REMType_View,
  REMType_ShaderModule,
  REMType_Buffer,
  REMType_Mbin,
  REMType_Gltf,
  REMType_Scene,
  REMType_Mesh,
  REMType_Shader,
  REMType_PointLight,
  REMType_AmbientLight,
  REMType_SpotLight,
  REMType_SunLight,
  REMType_PlaneReflection,
  REMType_ProbeReflection,
  REMType_ProbeReflectionGrid,
} REMType;

#define REM_STRUCT_ITEM(Name, Handle)                                          \
  typedef struct {                                                             \
    reg_id_t owner;                                                            \
    uint32_t key;                                                              \
    REMType type;                                                              \
    Handle handle;                                                             \
  } REM##Name;

REM_STRUCT_ITEM(Texture, WGPUTexture);
REM_STRUCT_ITEM(View, WGPUTextureView);
REM_STRUCT_ITEM(Buffer, WGPUBuffer);
REM_STRUCT_ITEM(ShaderModule, WGPUShaderModule);

REM_STRUCT_ITEM(Mesh, Mesh);
REM_STRUCT_ITEM(Scene, Scene);
REM_STRUCT_ITEM(Shader, Shader);

REM_STRUCT_ITEM(PointLight, PointLight);
REM_STRUCT_ITEM(AmbientLight, AmbientLight);
REM_STRUCT_ITEM(SpotLight, SpotLight);
REM_STRUCT_ITEM(SunLight, SunLight);

REM_STRUCT_ITEM(ProbeReflection, ProbeReflection);
REM_STRUCT_ITEM(PlaneReflection, ProbeReflectionPlane);
REM_STRUCT_ITEM(ProbeReflectionGrid, ProbeReflectionGrid);

REM_STRUCT_ITEM(Gltf, const char *);
REM_STRUCT_ITEM(Mbin, const char *);

#define REM_STRUCT_LIST(Type, VarName)                                         \
  struct {                                                                     \
    Type *entries;                                                             \
    size_t capacity;                                                           \
    size_t length;                                                             \
  } VarName;

typedef REM_STRUCT_LIST(void, REMList);

typedef struct ResourceManager {

  REMList entries[REM_TYPE_COUNT];

} ResourceManager;

extern ResourceManager g_rem;

static const struct {
  const char *label;
  const size_t capacity;
  const size_t type_size;
} rem_config[] = {
    [REMType_Texture] = {"Texture", 128, sizeof(REMTexture)},
    [REMType_View] = {"View", 128, sizeof(REMView)},
    [REMType_ShaderModule] = {"Shader Module", 128, sizeof(REMShaderModule)},
    [REMType_Buffer] = {"Buffer", 128, sizeof(REMBuffer)},
    [REMType_Mbin] = {"Mbin", 64, sizeof(REMMbin)},
    [REMType_Gltf] = {"Gltf", 64, sizeof(REMGltf)},
    [REMType_Scene] = {"Scene", 1, sizeof(REMScene)},
    [REMType_Mesh] = {"Mesh", 128, sizeof(REMMesh)},
    [REMType_Shader] = {"Shader", 128, sizeof(REMShader)},
    [REMType_PointLight] = {"Point Light", 128, sizeof(REMPointLight)},
    [REMType_AmbientLight] = {"Ambient Light", 32, sizeof(REMAmbientLight)},
    [REMType_SpotLight] = {"Spot Light", 32, sizeof(REMSpotLight)},
    [REMType_SunLight] = {"Sun Light", 32, sizeof(REMSunLight)},
    [REMType_PlaneReflection] = {"Plane Reflection", 16,
                                 sizeof(REMPlaneReflection)},
    [REMType_ProbeReflection] = {"Probe Reflection", 16,
                                 sizeof(REMProbeReflection)},
    [REMType_ProbeReflectionGrid] = {"Probe Reflection Grid", 16,
                                     sizeof(REMProbeReflectionGrid)},
};

typedef enum {
  REMTextureMemory_Keep,
  REMTextureMemory_Free,
} REMTextureMemory;

REMStatus resource_manager_init();

WGPUTexture rem_new_texture(const WGPUTextureDescriptor *);

WGPUTextureView rem_new_view(const WGPUTexture,
                             const WGPUTextureViewDescriptor *);
WGPUBuffer rem_new_buffer(const WGPUBufferDescriptor *);
WGPUShaderModule rem_new_shader_module(const WGPUShaderModuleDescriptor *);

Mesh *rem_new_mesh();
Scene *rem_new_scene();
Shader *rem_new_shader();

PointLight *rem_new_point_light();
AmbientLight *rem_new_ambient_light();
SpotLight *rem_new_spot_light();
SunLight *rem_new_sun_light();

ProbeReflectionPlane *rem_new_plane_reflection();
ProbeReflection *rem_new_probe_reflection();
ProbeReflectionGrid *rem_new_probe_reflection_grid();

REMStatus rem_destroy_texture(WGPUTexture *);
REMStatus rem_destroy_view(WGPUTextureView *);
REMStatus rem_destroy_buffer(WGPUBuffer *);
REMStatus rem_destroy_shader_module(WGPUShaderModule *);

REMStatus rem_destroy_shader(Shader *);
REMStatus rem_destroy_mesh(Mesh *);
REMStatus rem_destroy_scene(Scene *);

REMStatus rem_destroy_mbin(const char *);
REMStatus rem_destroy_gltf(const char *);

REMStatus rem_destroy_point_light(PointLight *);
REMStatus rem_destroy_ambient_light(AmbientLight *);
REMStatus rem_destroy_sun_light(SunLight *);
REMStatus rem_destroy_spot_light(SpotLight *);

REMStatus rem_destroy_plane_reflection(ProbeReflectionPlane *);
REMStatus rem_destroy_probe_reflection(ProbeReflection *);
REMStatus rem_destroy_probe_reflection_grid(ProbeReflectionGrid *);

// DELETEME

typedef struct {
  void *data;
  size_t size;
  WGPUBufferUsage usage;
  WGPUBool mappedAtCreation;
  const char *label;
} REMBufferDescriptor;

typedef struct {
  uint32_t width;
  uint32_t height;
  unsigned char *data;
  uint8_t channels;
  WGPUTextureFormat format;
  size_t size;
} REMTextureDescriptor;

typedef struct {
  const WGPUTexture texture;
  unsigned char *data;
  uint32_t width;
  uint32_t height;
  uint32_t layer;
  uint8_t channels;
  WGPUTextureFormat format;
  size_t size;
} REMTextureCubeDescriptor;

static inline WGPUTexture rem_create_texture_write(const REMTextureDescriptor *,
                                                   REMTextureMemory,
                                                   WGPUTextureView *);

static inline WGPUTexture
rem_create_texture_cube_write(const REMTextureCubeDescriptor *);

static inline WGPUBuffer rem_create_buffer_write(const REMBufferDescriptor *);

#endif
