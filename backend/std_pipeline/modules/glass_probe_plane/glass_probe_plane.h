#ifndef _PIPELINE_LAYOUT_GLASS_PROBE_PLANE_H_
#define _PIPELINE_LAYOUT_GLASS_PROBE_PLANE_H_

#include "../../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/probe/reflection/plane.h"
#include "../runtime/viewport/viewport.h"

#include "../glass_probe_grid/glass_probe_grid.h"

#include <webgpu/webgpu.h>

static const WGPUBindGroupLayoutDescriptor glass_probe_plane_bind_group = {
    // Group 1 (Reflection probes array + sampler)
    .label = "Group 1 (Reflection Probes)",
    .entryCount = 7,
    .entries =
        (WGPUBindGroupLayoutEntry[]){
            {
                .binding = 0, // uGlass
                .visibility = WGPUShaderStage_Fragment,
                .buffer =
                    (WGPUBufferBindingLayout){
                        .type = WGPUBufferBindingType_Uniform,
                        .hasDynamicOffset = false,
                        .minBindingSize = sizeof(GlassUniform),
                    },
            },
            {
                .binding = 1, // uProbeReflectionList
                .visibility = WGPUShaderStage_Fragment,
                .buffer =
                    (WGPUBufferBindingLayout){
                        .type = WGPUBufferBindingType_Uniform,
                        .hasDynamicOffset = false,
                        .minBindingSize = sizeof(ProbeReflectionPlaneUniform),
                    },
            },
            {
                .binding = 2, // UBO
                .visibility = WGPUShaderStage_Fragment,
                .buffer =
                    (WGPUBufferBindingLayout){
                        .type = WGPUBufferBindingType_Uniform,
                        .hasDynamicOffset = false,
                        .minBindingSize = sizeof(UBOUniform),
                    },
            },
            {
                .binding = 3, // probe_reflection_maps
                .visibility = WGPUShaderStage_Fragment,
                .texture =
                    (WGPUTextureBindingLayout){
                        .sampleType = WGPUTextureSampleType_Float,
                        .viewDimension = WGPUTextureViewDimension_2DArray,
                        .multisampled = false,
                    },
            },
            {
                .binding = 4, // probe_reflection_sampler
                .visibility = WGPUShaderStage_Fragment,
                .sampler =
                    (WGPUSamplerBindingLayout){
                        .type = WGPUSamplerBindingType_Filtering,
                    },
            },
            {
                .binding = 5, // env_map
                .visibility = WGPUShaderStage_Fragment,
                .texture =
                    (WGPUTextureBindingLayout){
                        .sampleType = WGPUTextureSampleType_Float,
                        .viewDimension = WGPUTextureViewDimension_Cube,
                        .multisampled = false,
                    },
            },
            {
                .binding = 6, // env_sampler
                .visibility = WGPUShaderStage_Fragment,
                .sampler =
                    (WGPUSamplerBindingLayout){
                        .type = WGPUSamplerBindingType_Filtering,
                    },
            },
        },
};

static const PipelineBindingProbe glass_probe_plane = {
    .group = 1,

    .reflection_plane = 1,
    .reflection_plane_texture = 3,
    .reflection_plane_sampler = 4,

    .reflection_grid = PIPELINE_BINDING_UNDEFINED,
    .reflection_grid_texture = PIPELINE_BINDING_UNDEFINED,
    .reflection_grid_sampler = PIPELINE_BINDING_UNDEFINED,

    .irradiance = PIPELINE_BINDING_UNDEFINED,
    .irradiance_sampler = PIPELINE_BINDING_UNDEFINED,
    .irradiance_texture = PIPELINE_BINDING_UNDEFINED,

    .skybox_texture = 5,
    .skybox_sampler = 6,

};

static const ShaderPipelineStateObject layout_glass_probe_plane = {
    .label = "Pipeline Bind Groups - Glass Probe Plane",
    .shader_path = "./backend/std_pipeline/modules/glass_probe_plane/"
                   "glass_probe_plane.wgsl",
    .bind_groups_count = 2,
    .bind_groups = {&mvp_layout, &glass_probe_plane_bind_group},
    .bindings = {.mvp = &mvp_binding, .probe = &glass_probe_plane},
};

#endif
