#ifndef _PIPELINE_LAYOUT_DEFAULT_H_
#define _PIPELINE_LAYOUT_DEFAULT_H_

#include "../../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include "../commons.h"
#include <webgpu/webgpu.h>

static const ShaderPipelineStateObject layout_default = {
    .label = "Pipeline Bind Groups - Default",
    .shader_path =
        "./backend/renderer/scene/std_pipeline/modules/default/default.wgsl",
    .bind_groups_count = 1,
    .bind_groups = {&mvp_layout},
    .bindings = {.mvp = &mvp_binding},
};

#endif
