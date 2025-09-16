#ifndef _PIPELINE_LAYOUT_UNLIT_H_
#define _PIPELINE_LAYOUT_UNLIT_H_

#include "runtime/pipeline/pipeline.h"
#include "runtime/camera/camera.h"
#include "runtime/mesh/mesh.h"
#include "runtime/viewport/viewport.h"

#include "../commons.h"
#include <webgpu/webgpu.h>

static const WGPUBindGroupLayoutDescriptor unlint_layout_bind_group = {
    // Group 1 (Billboard Material)
    .label = "Group 1 - Billboard Properties",
    .entryCount = 2,
    .entries =
        (WGPUBindGroupLayoutEntry[]){
            {
                .binding = 0, // uColor
                .visibility = WGPUShaderStage_Fragment,
                .buffer =
                    (WGPUBufferBindingLayout){
                        .type = WGPUBufferBindingType_Uniform,
                        .hasDynamicOffset = false,
                        .minBindingSize = sizeof(float) * 4, // vec4<f32>
                    },
            },
            {
                .binding = 1, // uFixedScale
                .visibility = WGPUShaderStage_Vertex,
                .buffer =
                    (WGPUBufferBindingLayout){
                        .type = WGPUBufferBindingType_Uniform,
                        .hasDynamicOffset = false,
                        .minBindingSize = sizeof(float), // f32
                    },
            },
        },
};

static const RenderPipelineStateObject layout_unlit = {
    .label = "Pipeline Bind Groups - Unlit",
    .shader_path =
        "./backend/std_pipeline/render_shader/unlit/unlit.wgsl",
    .bind_groups_count = 2,
    .bind_groups = {&mvp_layout, &unlint_layout_bind_group},
    .bindings = {.mvp = &mvp_binding},
};

#endif
