#ifndef _RENDERER_STD_PIPELINE_CORE_H_
#define _RENDERER_STD_PIPELINE_CORE_H_

#include <stdint.h>
#include <webgpu/webgpu.h>

#include "runtime/pipeline/pipeline.h"

// Standards pipelines
#define RENDER_PIPELINE_TYPE_COUNT 20
#define RENDER_PIPELINE_UNDEFINED FLT_MAX

typedef enum {
  RenderPipelineType_Billboard,
  RenderPipelineType_Default,
  RenderPipelineType_Grid,
  RenderPipelineType_Line,
  RenderPipelineType_PBR,
  RenderPipelineType_PBR_DoubleSided,
  RenderPipelineType_PBR_Alpha,
  RenderPipelineType_Screen,
  RenderPipelineType_Shadow,
  RenderPipelineType_Skybox,
  RenderPipelineType_Solid,
  RenderPipelineType_Unlit,
  RenderPipelineType_GlassProbeGrid,
  RenderPipelineType_GlassProbePlane,
  RenderPipelineType_Reflection,
  RenderPipelineType_Blit,
  RenderPipelineType_Outline,
  RenderPipelineType_Stencil,
  RenderPipelineType_Bloom,
  RenderPipelineType_Composite,
} RenderPipelineType;

#define COMPUTE_PIPELINE_TYPE_COUNT 2
typedef enum {
  ComputePipelineType_Mipmap,
  ComputePipelineType_Kawase,
} ComputePipelineType;

extern RenderPipeline *g_std_render_pipelines[RENDER_PIPELINE_TYPE_COUNT];
extern ComputePipeline *g_std_compute_pipelines[COMPUTE_PIPELINE_TYPE_COUNT];

EXTERN_C_BEGIN

void standard_render_pipelines_init(const RenderPipelineMultisampleCount);
void standard_render_pipelines_destroy();
void standard_compute_pipelines_init();

static inline const RenderPipeline *
std_render_pipeline(const RenderPipelineType type) {
  return g_std_render_pipelines[type];
}

static inline const ComputePipeline *
std_compute_pipeline(const ComputePipelineType type) {
  return g_std_compute_pipelines[type];
}

static inline RenderPipelineType
std_render_pipeline_type(const RenderPipeline *pipeline) {

  for (uint8_t i = 0; i < RENDER_PIPELINE_TYPE_COUNT; i++) {
    if (std_render_pipeline((RenderPipelineType)i) == pipeline)
      return (RenderPipelineType)i;
  }

  return (RenderPipelineType)RENDER_PIPELINE_UNDEFINED;
}

EXTERN_C_END

#endif
