#ifndef _RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGER_H_

#include "backend/registry.h"
#include "webgpu/webgpu.h"

static const size_t REM_TEXTURE_CAPACITY = 256;
static const size_t REM_SHADER_CAPACITY = 128;
static const size_t REM_VIEW_CAPACITY = 1024;
static const size_t REM_BUFFER_CAPACITY = 1024;
static const size_t REM_MBIN_CAPACITY = 32;
static const size_t REM_GLTF_CAPACITY = 32;

typedef enum {
  REMStatus_Success,
  REMStatus_MaxCapacity,
  REMStatus_UndefError,
} REMStatus;

typedef struct {
  reg_id_t scene_id;
  WGPUTexture handle;
  uint32_t key;
} REMTexture;

typedef struct {
  reg_id_t scene_id;
  WGPUTextureView handle;
  uint32_t key;
} REMView;

typedef struct {
  reg_id_t scene_id;
  WGPUShaderModule handle;
  uint32_t key;
} REMShader;

typedef struct {
  reg_id_t scene_id;
  WGPUBuffer handle;
  uint32_t key;
} REMBuffer;

typedef struct {
  reg_id_t scene_id;
  const char *handle;
  uint32_t key;
} REMMBIN;

typedef struct {
  reg_id_t scene_id;
  const char *handle;
  uint32_t key;
} REMGLTF;

typedef struct {

  struct {
    WGPUTexture entries[REM_TEXTURE_CAPACITY];
    size_t length;
  } texture;

  struct {
    WGPUTextureView entries[REM_VIEW_CAPACITY];
    size_t length;
  } view;

  struct {
    WGPUShaderModule entries[REM_SHADER_CAPACITY];
    size_t length;
  } shader;

  struct {
    WGPUBuffer entries[REM_BUFFER_CAPACITY];
    size_t length;
  } buffer;

  struct {
    WGPUBuffer entries[REM_MBIN_CAPACITY];
    size_t length;
  } mbin;

  struct {
    WGPUBuffer entries[REM_GLTF_CAPACITY];
    size_t length;
  } gltf;

} ResourceManager;

extern ResourceManager g_rem;

typedef enum {
  REMTextureMemory_Keep,
  REMTextureMemory_Free,
} REMTextureMemory;

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

WGPUTexture rem_create_texture(ResourceManager *,
                               const WGPUTextureDescriptor *);

WGPUTexture rem_create_texture_write(ResourceManager *,
                                     const REMTextureDescriptor *,
                                     REMTextureMemory, WGPUTextureView *);

WGPUTexture rem_create_texture_cube_write(ResourceManager *,
                                          const REMTextureCubeDescriptor *);

WGPUTextureView rem_create_view(ResourceManager *,
                                const WGPUTextureViewDescriptor *);

WGPUBuffer rem_create_buffer(ResourceManager *, const WGPUBufferDescriptor *);

WGPUBuffer rem_create_buffer_write(ResourceManager *,
                                   const REMBufferDescriptor *);

WGPUShaderModule rem_create_shader(ResourceManager *, const char *label,
                                   const char *code);

REMStatus rem_destroy_texture(ResourceManager *, WGPUTexture *);
REMStatus rem_destroy_view(ResourceManager *, WGPUTextureView *);
REMStatus rem_destroy_buffer(ResourceManager *, WGPUBuffer *);
REMStatus rem_destroy_shader(ResourceManager *, WGPUShaderModule *);

#endif
