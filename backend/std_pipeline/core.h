#ifndef _SCENE_RENDERER_STD_PIPELINE_CORE_H_
#define _SCENE_RENDERER_STD_PIPELINE_CORE_H_

#include <webgpu/webgpu.h>

#include "runtime/pipeline/pipeline.h"
#include "runtime/pipeline/core.h"

extern Pipeline g_std_pipelines[PIPELINE_TYPE_COUNT];

void standard_pipelines_init(const WGPUDevice, const PipelineMultisampleCount);

const Pipeline *std_pipeline(const PipelineType);

#endif
