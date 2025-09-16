#ifndef _PIPELINE_LAYOUT_DEFAULT_H_
#define _PIPELINE_LAYOUT_DEFAULT_H_

#include "runtime/pipeline/core.h"
#include "runtime/camera/camera.h"
#include "runtime/mesh/mesh.h"
#include "runtime/viewport/viewport.h"

#include "../commons.h"
#include <webgpu/webgpu.h>

static const RenderPipelineStateObject layout_default = {
    .label = "Pipeline Bind Groups - Default",
    .shader_path =
        "./backend/std_pipeline/render_shader/default/default.wgsl",
    .bind_groups_count = 1,
    .bind_groups = {&mvp_layout},
    .bindings = {.mvp = &mvp_binding},
};

#endif
