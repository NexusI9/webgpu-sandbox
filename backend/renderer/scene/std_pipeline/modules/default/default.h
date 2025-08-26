#ifndef _PIPELINE_LAYOUT_DEFAULT_H_
#define _PIPELINE_LAYOUT_DEFAULT_H_

#include "../../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include <webgpu/webgpu.h>

static const ShaderPipelineStateObject layout_default = {
    .label = "Pipeline Bind Groups - Default",
    .shader_path =
        "./backend/renderer/scene/std_pipeline/modules/default/default.wgsl",
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
                                    .type = WGPUBufferBindingType_ReadOnlyStorage,
                                    .hasDynamicOffset = true,
                                    .minBindingSize =
                                        sizeof(ViewportUniform) * SSBO_CAPACITY,
                                },
                        },
                        {
                            .binding = 1,
                            .visibility = WGPUShaderStage_Vertex |
                                          WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_ReadOnlyStorage,
                                    .hasDynamicOffset = true,
                                    .minBindingSize =
                                        sizeof(CameraUniform) * SSBO_CAPACITY,
                                },
                        },
                        {
                            .binding = 2,
                            .visibility = WGPUShaderStage_Vertex |
                                          WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_ReadOnlyStorage,
                                    .hasDynamicOffset = true,
                                    .minBindingSize =
                                        sizeof(MeshUniform) * SSBO_CAPACITY,
                                },
                        },
                    },
            },
        },
    .bindings =
        {
            .mvp =
                {
                    .group = 0,
                    .projection = 0,
                    .view = 1,
                    .model = 2,
                },
        },
};

#endif
