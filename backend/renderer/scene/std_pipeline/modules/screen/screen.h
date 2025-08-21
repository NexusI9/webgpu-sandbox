#ifndef _PIPELINE_LAYOUT_SCREEN_H_
#define _PIPELINE_LAYOUT_SCREEN_H_

#include "../../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include <webgpu/webgpu.h>

static const PipelineLayoutDescriptor layout_screen = {
    .label = "Pipeline Bind Groups - Screen",
    .shader_path =
        "./backend/renderer/scene/std_pipeline/modules/screen/screen.wgsl",
    .bind_groups_count = 2,
    .bind_groups =
        (WGPUBindGroupLayoutDescriptor[]){
            {
                // Group 0: Mesh, Viewport, Camera
                .label = "Group 0 - Mesh, Viewport, Camera",
                .entryCount = 3,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            // uMesh
                            .binding = 0,
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
                            // uViewport
                            .binding = 1,
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
                            // uCamera
                            .binding = 2,
                            .visibility = WGPUShaderStage_Vertex |
                                          WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(CameraUniform),
                                },
                        },
                    },
            },
            {
                // Group 1: Texture + Sampler
                .label = "Group 1 - Screen Texture",
                .entryCount = 2,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            // texture
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
                            // texture_sampler
                            .binding = 1,
                            .visibility = WGPUShaderStage_Fragment,
                            .sampler =
                                (WGPUSamplerBindingLayout){
                                    .type = WGPUSamplerBindingType_Filtering,
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
