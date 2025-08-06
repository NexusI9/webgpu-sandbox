#ifndef _PIPELINE_LAYOUT_SKYBOX_H_
#define _PIPELINE_LAYOUT_SKYBOX_H_

#include "../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include <webgpu/webgpu.h>

static const PipelineLayoutDescriptor layout_skybox = {
    .label = "Pipeline Bind Groups - Skybox",
    .shader_path =
        "./backend/renderer/scene/std_pipeline/modules/shader.skybox.wgsl",
    .bind_groups_count = 2,
    .bind_groups =
        (WGPUBindGroupLayoutDescriptor[]){
            {
                // Group 0: Skybox resources (texture, sampler, blur factor)
                .label = "Group 0 - Skybox Resources",
                .entryCount = 3,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            // skybox_texture
                            .binding = 0,
                            .visibility = WGPUShaderStage_Fragment,
                            .texture =
                                (WGPUTextureBindingLayout){
                                    .sampleType = WGPUTextureSampleType_Float,
                                    .viewDimension =
                                        WGPUTextureViewDimension_Cube,
                                    .multisampled = false,
                                },
                        },
                        {
                            // skybox_sampler
                            .binding = 1,
                            .visibility = WGPUShaderStage_Fragment,
                            .sampler =
                                (WGPUSamplerBindingLayout){
                                    .type = WGPUSamplerBindingType_Filtering,
                                },
                        },
                        {
                            // skybox_blur (float)
                            .binding = 2,
                            .visibility = WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(float),
                                },
                        },
                    },
            },
            {
                // Group 1: Camera & Mesh data
                .label = "Group 1 - Camera and Mesh",
                .entryCount = 3,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            // uViewport
                            .binding = 0,
                            .visibility = WGPUShaderStage_Vertex,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(ViewportUniform),
                                },
                        },
                        {
                            // uCamera
                            .binding = 1,
                            .visibility = WGPUShaderStage_Vertex,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(CameraUniform),
                                },
                        },
                        {
                            // uMesh
                            .binding = 2,
                            .visibility = WGPUShaderStage_Vertex,
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
    .custom_attributes =
        {
            // set cull to front face (inside cube)
            .primitive_state =
                (WGPUPrimitiveState){
                    .frontFace = WGPUFrontFace_CCW,
                    .cullMode = WGPUCullMode_Front,
                    .topology = WGPUPrimitiveTopology_TriangleList,
                    .stripIndexFormat = WGPUIndexFormat_Undefined,
                },
            // remove depth write, set depth comparison
            .stencil_state =
                (WGPUDepthStencilState){
                    .depthWriteEnabled = false,
                    .depthCompare = WGPUCompareFunction_LessEqual,
                    .format = WGPUTextureFormat_Depth24Plus,
                },
        },

};

#endif
