#ifndef _SCENE_RENDERER_STD_PIPELINE_CORE_H_
#define _SCENE_RENDERER_STD_PIPELINE_CORE_H_

#include <webgpu/webgpu.h>

#include "runtime/pipeline/pipeline.h"
#include "runtime/pipeline/core.h"

// Standards pipelines
#define RENDER_PIPELINE_TYPE_COUNT 14
#define COMPUTE_PIPELINE_TYPE_COUNT 1

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

typedef enum {
  ComputePipelineType_Mipmaps,
} ComputePipelineType;


extern Pipeline g_std_render_pipelines[RENDER_PIPELINE_TYPE_COUNT];
extern Pipeline g_std_compute_pipelines[COMPUTE_PIPELINE_TYPE_COUNT];

void standard_pipelines_init(const WGPUDevice, const PipelineMultisampleCount);

const Pipeline *std_render_pipeline(const RenderPipelineType);
const Pipeline *std_compute_pipeline(const RenderPipelineType);

#endif
