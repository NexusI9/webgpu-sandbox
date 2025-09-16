#ifndef _PIPELINE_LAYOUT_PBR_H_
#define _PIPELINE_LAYOUT_PBR_H_

#include "backend/ubo.h"
#include "runtime/camera/camera.h"
#include "runtime/light/light.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/mesh.h"
#include "runtime/pipeline/pipeline.h"
#include "runtime/probe/probe.h"
#include "runtime/viewport/viewport.h"

#include "../commons.h"
#include <webgpu/webgpu.h>

typedef struct {
  vec4 base_color_factor;
  vec3 emissive_factor;
  float metallic_factor;
  float roughness_factor;
  float specular_factor;
  float normal_scale;
  float occlusion_strength;
} PBRMaterialUniform;

static const WGPUBindGroupLayoutDescriptor layout_pbr_textures_bind_group = {
    // Group 1: Material Textures
    .label = "Group 1 - Material Textures",
    .entryCount = 11,
    .entries =
        (WGPUBindGroupLayoutEntry[]){
            {
                // diffuse_texture
                .binding = 0,
                .visibility = WGPUShaderStage_Fragment,
                .texture =
                    (WGPUTextureBindingLayout){
                        .sampleType = WGPUTextureSampleType_Float,
                        .viewDimension = WGPUTextureViewDimension_2D,
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
                        .viewDimension = WGPUTextureViewDimension_2D,
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
                        .viewDimension = WGPUTextureViewDimension_2D,
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
                        .viewDimension = WGPUTextureViewDimension_2D,
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
                        .viewDimension = WGPUTextureViewDimension_2D,
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
            {
                // PBR_uniform
                .binding = 10,
                .visibility = WGPUShaderStage_Fragment,
                .buffer =
                    (WGPUBufferBindingLayout){
                        .type = WGPUBufferBindingType_Uniform,
                        .hasDynamicOffset = false,
                        .minBindingSize = sizeof(PBRMaterialUniform)},
            },
        },
};

static const PipelineBindingLightList pbr_light_list = {
    .group = 2,
    .ambient = 0,
    .spot = 1,
    .point = 2,
    .sun = 3,
    .point_texture = 5,
    .directional_texture = 7,
};

static const PipelineBindingProbe pbr_probe = {
    .group = 2,

    .reflection_plane = 9,
    .reflection_plane_texture = 11,
    .reflection_plane_sampler = 12,

    .reflection_grid = 10,
    .reflection_grid_texture = 13,
    .reflection_grid_sampler = 14,

    .irradiance = PIPELINE_BINDING_UNDEFINED,
    .irradiance_sampler = PIPELINE_BINDING_UNDEFINED,
    .irradiance_texture = PIPELINE_BINDING_UNDEFINED,

    .skybox_texture = 15,
    .skybox_sampler = 16,
};

static const WGPUBindGroupLayoutDescriptor layout_pbr_lights_bind_group = {
    // Group 2: Lights + Shadows
    .label = "Group 2 - Lights and Shadows",
    .entryCount = 17,
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
                            sizeof(AmbientLightUniform) * SSBO_CAPACITY,
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
                            sizeof(SpotLightUniform) * SSBO_CAPACITY,
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
                            sizeof(PointLightUniform) * SSBO_CAPACITY,
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
                            sizeof(SunLightUniform) * SSBO_CAPACITY,
                    },
            },
            {
                // light_list_length
                .binding = 4,
                .visibility = WGPUShaderStage_Fragment,
                .buffer =
                    (WGPUBufferBindingLayout){
                        .type = WGPUBufferBindingType_Uniform,
                        .hasDynamicOffset = false,
                        .minBindingSize = sizeof(UBOUniform),
                    },
            },
            {
                // point_shadow_maps
                .binding = 5,
                .visibility = WGPUShaderStage_Fragment,
                .texture =
                    (WGPUTextureBindingLayout){
#ifdef RENDER_SHADOW_AS_COLOR
                        .sampleType = WGPUTextureSampleType_Float,
#else
                        .sampleType = WGPUTextureSampleType_Depth,
#endif
                        .viewDimension = WGPUTextureViewDimension_CubeArray,
                        .multisampled = false,
                    },
            },
            {
                // point_shadow_sampler
                .binding = 6,
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
                .binding = 7,
                .visibility = WGPUShaderStage_Fragment,
                .texture =
                    (WGPUTextureBindingLayout){
#ifdef RENDER_SHADOW_AS_COLOR
                        .sampleType = WGPUTextureSampleType_Float,
#else
                        .sampleType = WGPUTextureSampleType_Depth,
#endif
                        .viewDimension = WGPUTextureViewDimension_2DArray,
                        .multisampled = false,
                    },
            },
            {
                // directional_shadow_sampler
                .binding = 8,
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
                .binding = 9, // uProbeReflectionList
                .visibility = WGPUShaderStage_Fragment,
                .buffer =
                    (WGPUBufferBindingLayout){
                        .type = WGPUBufferBindingType_Uniform,
                        .hasDynamicOffset = true,
                        .minBindingSize = sizeof(ProbeReflectionPlaneUniform),
                    },
            },
            {
                .binding = 10, // uProbeReflectionList
                .visibility = WGPUShaderStage_Fragment,
                .buffer =
                    (WGPUBufferBindingLayout){
                        .type = WGPUBufferBindingType_Uniform,
                        .hasDynamicOffset = true,
                        .minBindingSize = sizeof(ProbeReflectionUniform),
                    },
            },
            {
                .binding = 11, // Probe Plane Texture
                .visibility = WGPUShaderStage_Fragment,
                .texture =
                    (WGPUTextureBindingLayout){
                        .sampleType = WGPUTextureSampleType_Float,
                        .viewDimension = WGPUTextureViewDimension_2DArray,
                        .multisampled = false,
                    },
            },
            {
                .binding = 12, // Probe Plane Sampler
                .visibility = WGPUShaderStage_Fragment,
                .sampler =
                    (WGPUSamplerBindingLayout){
                        .type = WGPUSamplerBindingType_Filtering,
                    },
            },
            {
                .binding = 13, // Probe Grid Texture
                .visibility = WGPUShaderStage_Fragment,
                .texture =
                    (WGPUTextureBindingLayout){
                        .sampleType = WGPUTextureSampleType_Float,
                        .viewDimension = WGPUTextureViewDimension_CubeArray,
                        .multisampled = false,
                    },
            },
            {
                .binding = 14, // Probe Grid Texture
                .visibility = WGPUShaderStage_Fragment,
                .sampler =
                    (WGPUSamplerBindingLayout){
                        .type = WGPUSamplerBindingType_Filtering,
                    },
            },
            {
                .binding = 15, // skybox_texture
                .visibility = WGPUShaderStage_Fragment,
                .texture =
                    (WGPUTextureBindingLayout){
                        .sampleType = WGPUTextureSampleType_Float,
                        .viewDimension = WGPUTextureViewDimension_Cube,
                        .multisampled = false,
                    },
            },
            {
                .binding = 16, // skybox_sampler
                .visibility = WGPUShaderStage_Fragment,
                .sampler =
                    (WGPUSamplerBindingLayout){
                        .type = WGPUSamplerBindingType_Filtering,
                    },
            },

        },
};

static const ShaderPipelineStateObject layout_pbr = {
    .label = "Pipeline Bind Groups - PBR",
    .shader_path = "../backend/std_pipeline/render_shader/pbr/pbr.wgsl",
    .bind_groups_count = 3,
    .bind_groups =
        {
            &mvp_layout,
            &layout_pbr_textures_bind_group,
            &layout_pbr_lights_bind_group,
        },
    .bindings =
        {
            .mvp = &mvp_binding,
            .light_list = &pbr_light_list,
            .probe = &pbr_probe,
        },
};

#endif
