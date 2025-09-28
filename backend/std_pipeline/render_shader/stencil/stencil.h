#ifndef _PIPELINE_LAYOUT_stencil_H_
#define _PIPELINE_LAYOUT_stencil_H_

#include "../commons.h"
#include <webgpu/webgpu.h>

static const RenderPipelineStateObject layout_stencil = {
    .label = "Pipeline Bind Groups - Stencil",
    .shader_path = "./backend/std_pipeline/render_shader/stencil/stencil.wgsl",
    .bind_groups_count = 1,
    .bind_groups = {&mvp_layout},
    .bindings = {.mvp = &mvp_binding},
    .pipeline_attributes = {.stencil_state = &selectable_stencil},
};

#endif
