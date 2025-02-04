#ifndef _SHADER_H_
#define _SHADER_H_

#include <stddef.h>
#include <stdint.h>

#define SHADER_GROUP_CAMERA 0
#define SHADER_GROUP_VIEWPORT 0
#define SHADER_BIND_CAMERA 0
#define SHADER_BIND_VIEWPORT 1
#define SHADER_MAX_BIND_GROUP 4

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
  const char *name;
} ShaderCreateDescriptor;

// core

typedef struct {
  WGPUBindGroupEntry items[SHADER_MAX_BIND_GROUP];
  size_t length;
} ShaderBindGroupEntries;

typedef struct {
  WGPUBuffer buffer;              // unform buffer
  void *data;                     // uniform data
  WGPUBindGroup bind_group;       // bind group
  uint8_t index;                  // bind group id
  ShaderBindGroupEntries entries; // entries (uniform)
} ShaderBindGroup;

typedef struct {
  char *source; // shader source code
  WGPUShaderModule module;
  const char *name;

  // wgpu
  WGPUDevice *device;
  WGPUQueue *queue;
  WGPURenderPipeline pipeline;

  // vertex data
  struct {
    WGPUVertexAttribute attribute[2];
    WGPUVertexBufferLayout layout;
  } vertex;

  // uniforms data along with userful information (buffer, group index...)
  // TODO: separate statics from dynamics
  // registered bind group unique indexes
  struct {
    ShaderBindGroup items[SHADER_MAX_BIND_GROUP];
    size_t length;
  } bind_groups;

  int8_t projection_view_bindgroup;

} shader;

// methods
shader shader_create(const ShaderCreateDescriptor *);
void shader_add_uniform(shader *, const ShaderCreateUniformDescriptor *);
void shader_draw(shader *, WGPURenderPassEncoder *, const camera *,
                 const viewport *);

void shader_bind_camera(shader *, camera *, viewport *, uint8_t);
void shader_release(shader *);
void shader_build(shader *);

#endif
