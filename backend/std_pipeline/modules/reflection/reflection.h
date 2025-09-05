#ifndef _PIPELINE_LAYOUT_REFLECTION_H_
#define _PIPELINE_LAYOUT_REFLECTION_H_

#include "../../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/light/uniform.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include "../commons.h"
#include "../pbr/pbr.h"
#include <webgpu/webgpu.h>

static const ShaderPipelineStateObject layout_reflection = {
    .label = "Pipeline Bind Groups - Reflection",
    .shader_path = "../backend/std_pipeline/modules/reflection/"
                   "reflection.wgsl",
    .bind_groups_count = 3,
    .bind_groups =
        {
            &mvp_layout,
            &layout_pbr_textures_bind_group,
            &layout_pbr_lights_bind_group,
        },
    .pipeline_attributes =
        {
            .multisample_state =
                (WGPUMultisampleState){
                    .alphaToCoverageEnabled = false,
                    .mask = 0xFFFFFFFF,
                    .count = PipelineMultisampleCount_1x,
                },
        },
    .bindings = {.mvp = &mvp_binding},
};

#endif
