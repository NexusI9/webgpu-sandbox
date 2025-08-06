#ifndef _SCENE_RENDERER_STD_PIPELINE_CORE_H_
#define _SCENE_RENDERER_STD_PIPELINE_CORE_H_

#include "../core.h"

void scene_renderer_init_standard_pipelines(SceneRenderer *renderer);

Pipeline *std_pipeline(SceneRenderer *, const PipelineType);

#endif
