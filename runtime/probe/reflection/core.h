#ifndef _PROBE_REFLECTION_CORE_H_
#define _PROBE_REFLECTION_CORE_H_

#include <stddef.h>
#include <stdint.h>

#include "../runtime/scene/renderer/renderer.h"
#include "../utils/vector/vector.h"
#include "webgpu/webgpu.h"
#include "../backend/ssbo.h"
#include "../runtime/mesh/core.h"
#include "../runtime/pipeline/core.h"
#include "../runtime/scene/debug/core.h"
#include "../runtime/scene/renderer/render_pass/core.h"
#include "../runtime/texture/core.h"
#include "../utils/dyli.h"

#define PROBE_REFLECTION_SSBO_SLOT_COUNT 2
#define PROBE_REFLECTION_NEAR 0.1f
#define PROBE_REFLECTION_FAR 100.0f

typedef enum {
  ProbeReflectionSSBOField_List,
  ProbeReflectionSSBOField_Camera,
} ProbeReflectionSSBOField;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  const RenderPassDrawListDescriptor *draw_list;
  const TextureResolution resolution;
  const size_t capacity;
  const PipelineMultisampleCount multisample;
} ProbeReflectionListDescriptor;

typedef struct {
  SceneDebug *scene_debug;
  const uint8_t max_views;
} ProbeReflectionListDebug;

typedef struct {
  ssbo_id_t camera_offset;
} ProbeReflectionListPreprocessorData;

typedef struct {
  WGPUTexture *color;
  WGPUTexture *depth;
  WGPUTextureView *color_view;
  WGPUTextureView *depth_view;
  const TextureResolution resolution;
  const WGPUDevice device;
  const size_t layer_count;
  const WGPUTextureViewDimension view_dimension;
} ProbeReflectionTextureDescriptor;

typedef struct {
  void **entries;
  size_t *capacity;
  size_t *length;
  const size_t num;
  const size_t type_size;
  RenderPass *pass;
  const char *label;
} ProbeReflectionCreateCoreList;

typedef struct {
  const size_t layer_count;
  const WGPUTextureViewDimension view_dimension;
  const TextureResolution resolution;
  const RenderPassDrawListDescriptor *draw_list;
  const PipelineMultisampleCount multisample;
  RenderPass *handle;
} ProbeReflectionCreateCorePass;

typedef struct {

  const WGPUDevice device;
  const WGPUQueue queue;

  ProbeReflectionCreateCoreList *probe_list;
  ProbeReflectionCreateCorePass *render_pass;

} ProbeReflectionCreateCore;

DynamicListStatus
probe_reflection_list_create_core(const ProbeReflectionCreateCore *);

void probe_reflection_list_draw_preprocessor(const RenderPass *, Mesh *,
                                             void *);

#endif
