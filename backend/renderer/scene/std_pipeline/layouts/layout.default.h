#ifndef _PIPELINE_LAYOUT_DEFAULT_H_
#define _PIPELINE_LAYOUT_DEFAULT_H_

#include "../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include <webgpu/webgpu.h>

static const PipelineLayoutDescriptor layout_default = {
    .label = "Pipeline Bind Groups - Default",
    .shader_path =
        "./backend/renderer/scene/std_pipeline/modules/shader.default.wgsl",
    .bind_groups_count = 1,
    .bind_groups =
        (WGPUBindGroupLayoutDescriptor[]){
            {
                // Group 0
                .label = "Group 0 (Scene Data)",
                .entryCount = 3,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            .binding = 0,
                            .visibility = WGPUShaderStage_Vertex |
                                          WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(ViewportUniform),
                                },
                        },
                        {
                            .binding = 1,
                            .visibility = WGPUShaderStage_Vertex |
                                          WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(CameraUniform),
                                },
                        },
                        {
                            .binding = 2,
                            .visibility = WGPUShaderStage_Vertex |
                                          WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(MeshUniform),
                                },
                        },
                    },
            },
        },
};

#endif
