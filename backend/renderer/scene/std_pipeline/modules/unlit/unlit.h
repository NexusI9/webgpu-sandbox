#ifndef _PIPELINE_LAYOUT_UNLIT_H_
#define _PIPELINE_LAYOUT_UNLIT_H_

#include "../../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include <webgpu/webgpu.h>

static const ShaderPipelineStateObject layout_unlit = {
    .label = "Pipeline Bind Groups - Unlit",
    .shader_path =
        "./backend/renderer/scene/std_pipeline/modules/unlit/unlit.wgsl",
    .bind_groups_count = 2,
    .bind_groups =
        (WGPUBindGroupLayoutDescriptor[]){
            {
                // Group 0 (Scene Data)
                .label = "Group 0 - Camera and Mesh",
                .entryCount = 3,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            .binding = 0, // uViewport
                            .visibility = WGPUShaderStage_Vertex |
                                          WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_ReadOnlyStorage,
                                    .hasDynamicOffset = true,
                                    .minBindingSize =
                                        sizeof(ViewportUniform),
                                },
                        },
                        {
                            .binding = 1, // uCamera
                            .visibility = WGPUShaderStage_Vertex |
                                          WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_ReadOnlyStorage,
                                    .hasDynamicOffset = true,
                                    .minBindingSize =
                                        sizeof(CameraUniform),
                                },
                        },
                        {
                            .binding = 2, // uMesh
                            .visibility = WGPUShaderStage_Vertex |
                                          WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_ReadOnlyStorage,
                                    .hasDynamicOffset = true,
                                    .minBindingSize =
                                        sizeof(MeshUniform),
                                },
                        },
                    },
            },
            {
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
                                    .minBindingSize =
                                        sizeof(float) * 4, // vec4<f32>
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
