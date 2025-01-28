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
  uint8_t group_index;
  uint8_t entry_count;
  WGPUBindGroupEntry *entries;
} ShaderBindGroupDescriptor;

typedef struct {
  char *path;
  const char *label;
  WGPUDevice *device;
  camera camera;
  viewport viewport;
} ShaderCreateDescriptor;

// core
typedef struct {
  WGPUBindGroup items[SHADER_MAX_BIND_GROUP];
  size_t length;
} ShaderBindGroupList;

typedef struct {
  char *source; // shader source code
  WGPUShaderModule module;
  ShaderBindGroupList bind_group_list; // TODO: separate statics from dynamics
                                       // (often updated) BindGroups
  WGPUDevice *device;
  WGPUVertexBufferLayout vertex_layout;
  WGPURenderPipeline pipeline;
} shader;

// methods
shader shader_create(const ShaderCreateDescriptor *);
WGPUBindGroup shader_add_bind_group(shader *,
                                    const ShaderBindGroupDescriptor *);

void shader_draw(const shader *, WGPURenderPassEncoder *, const camera *,
                 const viewport *);
#endif
