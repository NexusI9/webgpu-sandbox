#ifndef _PIPELINE_LAYOUT_LINE_H_
#define _PIPELINE_LAYOUT_LINE_H_

#include "../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include <webgpu/webgpu.h>

static const PipelineLayoutDescriptor layout_line = {
    .label = "Pipeline Bind Groups - Line",
    .shader_path =
        "./backend/renderer/scene/std_pipeline/modules/shader.line.wgsl",
    .bind_groups_count = 1,
    .bind_groups =
        (WGPUBindGroupLayoutDescriptor[]){
            {
                // Group 0
                .label = "Group 0 - Viewport, Camera, Mesh",
                .entryCount = 4,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            .binding = 0, // uViewport
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
                            .binding = 1, // uCamera
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
                            .binding = 2, // uMesh
                            .visibility = WGPUShaderStage_Vertex |
                                          WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(MeshUniform),
                                },
                        },
                        {
                            .binding = 3, // Color
                            .visibility = WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(color),
                                },
                        },
                    },
            },
        },
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
