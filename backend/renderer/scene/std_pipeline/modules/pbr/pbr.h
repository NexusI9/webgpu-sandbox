#ifndef _PIPELINE_LAYOUT_PBR_H_
#define _PIPELINE_LAYOUT_PBR_H_

#include "../../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/light/uniform.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include <webgpu/webgpu.h>

static const ShaderPipelineStateObject layout_pbr = {
    .label = "Pipeline Bind Groups - PBR",
    .shader_path =
        "../backend/renderer/scene/std_pipeline/modules/pbr/pbr.wgsl",
    .bind_groups_count = 3,
    .bind_groups =
        (WGPUBindGroupLayoutDescriptor[]){
            {
                // Group 0: Camera, Mesh
                .label = "Group 0 - Camera, Viewport, Mesh",
                .entryCount = 3,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            // uViewport
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
                            // uCamera
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
                            // uMesh
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
            {
                // Group 1: Material Textures
                .label = "Group 1 - Material Textures",
                .entryCount = 10,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            // diffuse_texture
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
                            // diffuse_sampler
                            .binding = 1,
                            .visibility = WGPUShaderStage_Fragment,
                            .sampler =
                                (WGPUSamplerBindingLayout){
                                    .type = WGPUSamplerBindingType_Filtering,
                                },
                        },
                        {
                            // metallic_texture
                            .binding = 2,
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
                            // metallic_sampler
                            .binding = 3,
                            .visibility = WGPUShaderStage_Fragment,
                            .sampler =
                                (WGPUSamplerBindingLayout){
                                    .type = WGPUSamplerBindingType_Filtering,
                                },
                        },
                        {
                            // normal_texture
                            .binding = 4,
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
                            // normal_sampler
                            .binding = 5,
                            .visibility = WGPUShaderStage_Fragment,
                            .sampler =
                                (WGPUSamplerBindingLayout){
                                    .type = WGPUSamplerBindingType_Filtering,
                                },
                        },
                        {
                            // emissive_texture
                            .binding = 6,
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
                            // emissive_sampler
                            .binding = 7,
                            .visibility = WGPUShaderStage_Fragment,
                            .sampler =
                                (WGPUSamplerBindingLayout){
                                    .type = WGPUSamplerBindingType_Filtering,
                                },
                        },
                        {
                            // occlusion_texture
                            .binding = 8,
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
                            // occlusion_sampler
                            .binding = 9,
                            .visibility = WGPUShaderStage_Fragment,
                            .sampler =
                                (WGPUSamplerBindingLayout){
                                    .type = WGPUSamplerBindingType_Filtering,
                                },
                        },
                    },
            },
            {
                // Group 2: Lights + Shadows
                .label = "Group 2 - Lights and Shadows",
                .entryCount = 8,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            // ambient_light_list
                            .binding = 0,
                            .visibility = WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize =
                                        sizeof(AmbientLightListUniform),
                                },
                        },
                        {
                            // spot_light_list
                            .binding = 1,
                            .visibility = WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize =
                                        sizeof(SpotLightListUniform),
                                },
                        },
                        {
                            // point_light_list
                            .binding = 2,
                            .visibility = WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize =
                                        sizeof(PointLightListUniform),
                                },
                        },
                        {
                            // sun_light_list
                            .binding = 3,
                            .visibility = WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize =
                                        sizeof(SunLightListUniform),
                                },
                        },
                        {
                            // point_shadow_maps
                            .binding = 4,
                            .visibility = WGPUShaderStage_Fragment,
                            .texture =
                                (WGPUTextureBindingLayout){
#ifdef RENDER_SHADOW_AS_COLOR
                                    .sampleType = WGPUTextureSampleType_Float,
#else
                                    .sampleType = WGPUTextureSampleType_Depth,
#endif
                                    .viewDimension =
                                        WGPUTextureViewDimension_CubeArray,
                                    .multisampled = false,
                                },
                        },
                        {
                            // point_shadow_sampler
                            .binding = 5,
                            .visibility = WGPUShaderStage_Fragment,
                            .sampler =
                                (WGPUSamplerBindingLayout){
#ifdef RENDER_SHADOW_AS_COLOR
                                    .type = WGPUSamplerBindingType_Filtering,
#else
                                    .type = WGPUSamplerBindingType_Comparison,
#endif
                                },
                        },
                        {
                            // directional_shadow_maps
                            .binding = 6,
                            .visibility = WGPUShaderStage_Fragment,
                            .texture =
                                (WGPUTextureBindingLayout){
#ifdef RENDER_SHADOW_AS_COLOR
                                    .sampleType = WGPUTextureSampleType_Float,
#else
                                    .sampleType = WGPUTextureSampleType_Depth,
#endif
                                    .viewDimension =
                                        WGPUTextureViewDimension_2DArray,
                                    .multisampled = false,
                                },
                        },
                        {
                            // directional_shadow_sampler
                            .binding = 7,
                            .visibility = WGPUShaderStage_Fragment,
                            .sampler =
                                (WGPUSamplerBindingLayout){
#ifdef RENDER_SHADOW_AS_COLOR
                                    .type = WGPUSamplerBindingType_Filtering,
#else
                                    .type = WGPUSamplerBindingType_Comparison,
#endif
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
