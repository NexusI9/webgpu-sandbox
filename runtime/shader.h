#ifndef _SHADER_H_
#define _SHADER_H_

#include <stddef.h>
#include <stdint.h>

#define SHADER_GROUP_CAMERA 0
#define SHADER_GROUP_VIEWPORT 0
#define SHADER_BIND_CAMERA 0
#define SHADER_BIND_VIEWPORT 1
#define SHADER_MAX_BIND_GROUP 32

#include "camera.h"
#include "viewport.h"
#include "webgpu/webgpu.h"

// descriptors
typedef struct {
  uint64_t offset;
  uint64_t size;
  uint32_t binding;
} ShaderUniformEntry;

typedef struct {
  uint8_t group_index;
  uint8_t entry_count;
  WGPUBindGroupEntry *entries;
  void *data;
  size_t size;
} ShaderCreateUniformDescriptor;

typedef struct {
  char *path;
  const char *label;
  WGPUDevice *device;
  WGPUQueue *queue;
} ShaderCreateDescriptor;

// core
typedef struct {
  WGPUBuffer buffer;
  void *data;
  WGPUBindGroup bind_group;
} ShaderUniforms;

typedef struct {
  char *source; // shader source code
  WGPUShaderModule module;

  // wgpu
  WGPUDevice *device;
  WGPUQueue *queue;
  WGPURenderPipeline pipeline;

  // vertex data
  struct {
    WGPUVertexAttribute attribute[2];
    WGPUVertexBufferLayout layout;
  } vertex;

  // uniforms
  // TODO: separate statics from dynamics
  // (often updated) BindGroups

  struct {
    ShaderUniforms items[SHADER_MAX_BIND_GROUP];
    size_t length;
  } uniforms;

} shader;

// methods
shader shader_create(const ShaderCreateDescriptor *);
WGPUBindGroup shader_add_uniform(shader *,
                                 const ShaderCreateUniformDescriptor *);
void shader_draw(const shader *, WGPURenderPassEncoder *, const camera *,
                 const viewport *);

#endif
