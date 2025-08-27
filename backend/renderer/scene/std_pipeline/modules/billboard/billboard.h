#ifndef _PIPELINE_LAYOUT_BILLBOARD_H_
#define _PIPELINE_LAYOUT_BILLBOARD_H_

#include "../../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include <webgpu/webgpu.h>

static const ShaderPipelineStateObject layout_billboard = {
    .label = "Pipeline Bind Groups - Billboard",
    .shader_path = "./backend/renderer/scene/std_pipeline/modules/billboard/"
                   "billboard.wgsl",
    .bind_groups_count = 2,
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
                                    .type =
                                        WGPUBufferBindingType_ReadOnlyStorage,
                                    .hasDynamicOffset = true,
                                    .minBindingSize = sizeof(ViewportUniform),
                                },
                        },
                        {
                            .binding = 1,
                            .visibility = WGPUShaderStage_Vertex |
                                          WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type =
                                        WGPUBufferBindingType_ReadOnlyStorage,
                                    .hasDynamicOffset = true,
                                    .minBindingSize = sizeof(CameraUniform),
                                },
                        },
                        {
                            .binding = 2,
                            .visibility = WGPUShaderStage_Vertex |
                                          WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type =
                                        WGPUBufferBindingType_ReadOnlyStorage,
                                    .hasDynamicOffset = true,
                                    .minBindingSize = sizeof(MeshUniform),
                                },
                        },
                    },
            },
            {
                // Group 1
                .label = "Group 1 (Material Data)",
                .entryCount = 3,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            .binding = 0,
                            .visibility = WGPUShaderStage_Fragment,
                            .texture =
                                (WGPUTextureBindingLayout){
                                    .sampleType = WGPUTextureSampleType_Float,
                                    .viewDimension =
                                        WGPUTextureViewDimension_2D,
                                    .multisampled = false,
                                },
                        },
                        {
                            .binding = 1,
                            .visibility = WGPUShaderStage_Fragment,
                            .sampler =
                                (WGPUSamplerBindingLayout){
                                    .type = WGPUSamplerBindingType_Filtering,
                                },
                        },
                        {
                            .binding = 2,
                            .visibility = WGPUShaderStage_Fragment |
                                          WGPUShaderStage_Vertex,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize =
                                        sizeof(uint32_t), // or 256-byte aligned
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
