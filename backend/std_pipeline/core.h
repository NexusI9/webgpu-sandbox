#ifndef _SCENE_RENDERER_STD_PIPELINE_CORE_H_
#define _SCENE_RENDERER_STD_PIPELINE_CORE_H_

#include <webgpu/webgpu.h>

#include "runtime/pipeline/pipeline.h"

// Standards pipelines
#define RENDER_PIPELINE_TYPE_COUNT 14
typedef enum {
  RenderPipelineType_Billboard,
  RenderPipelineType_Default,
  RenderPipelineType_Grid,
  RenderPipelineType_Line,
  RenderPipelineType_PBR,
  RenderPipelineType_Screen,
  RenderPipelineType_Shadow,
  RenderPipelineType_ShadowCullBack,
  RenderPipelineType_Skybox,
  RenderPipelineType_Solid,
  RenderPipelineType_Unlit,
  RenderPipelineType_GlassProbeGrid,
  RenderPipelineType_GlassProbePlane,
  RenderPipelineType_Reflection,
} RenderPipelineType;

#define COMPUTE_PIPELINE_TYPE_COUNT 2
typedef enum {
  ComputePipelineType_Mipmap,
  ComputePipelineType_Kawase,
} ComputePipelineType;

extern RenderPipeline g_std_render_pipelines[RENDER_PIPELINE_TYPE_COUNT];
extern ComputePipeline g_std_compute_pipelines[COMPUTE_PIPELINE_TYPE_COUNT];

void standard_render_pipelines_init(const WGPUDevice,
                                    const RenderPipelineMultisampleCount);
void standard_compute_pipelines_init(const WGPUDevice);

static inline const RenderPipeline *
std_render_pipeline(const RenderPipelineType type) {
  return &g_std_render_pipelines[type];
}

static inline const ComputePipeline *
std_compute_pipeline(const ComputePipelineType type) {
  return &g_std_compute_pipelines[type];
}

#endif
