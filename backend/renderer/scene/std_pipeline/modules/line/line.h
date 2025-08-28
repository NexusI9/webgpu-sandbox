#ifndef _PIPELINE_LAYOUT_LINE_H_
#define _PIPELINE_LAYOUT_LINE_H_

#include "../../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include "../commons.h"
#include <webgpu/webgpu.h>

static const WGPUBindGroupLayoutDescriptor line_bind_group = {
    // Group 1
    .label = "Group 1 - Color",
    .entryCount = 1,
    .entries =
        (WGPUBindGroupLayoutEntry[]){
            {
                .binding = 0, // Color
                .visibility = WGPUShaderStage_Fragment,
                .buffer =
                    (WGPUBufferBindingLayout){
                        .type = WGPUBufferBindingType_Uniform,
                        .hasDynamicOffset = false,
                        .minBindingSize = sizeof(color),
                    },
            },
        },
};

static const ShaderPipelineStateObject layout_line = {
    .label = "Pipeline Bind Groups - Line",
    .shader_path =
        "./backend/renderer/scene/std_pipeline/modules/line/line.wgsl",
    .bind_groups_count = 2,
    .bind_groups = {&mvp_layout, &line_bind_group},
    .pipeline_attributes =
        {
            .primitive_state =
                (WGPUPrimitiveState){
                    .frontFace = WGPUFrontFace_CCW,
                    .cullMode = WGPUCullMode_None,
                    .topology = WGPUPrimitiveTopology_TriangleList,
                    .stripIndexFormat = WGPUIndexFormat_Undefined,
                },
        },
    .bindings = {.mvp = &mvp_binding},
};

#endif
