#ifndef _PIPELINE_LAYOUT_GLASS_PROBE_GRID_H_
#define _PIPELINE_LAYOUT_GLASS_PROBE_GRID_H_

#include "../../core.h"
#include "../glass/glass.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/probe/reflection/grid.h"
#include "../runtime/viewport/viewport.h"

#include <webgpu/webgpu.h>

static const WGPUBindGroupLayoutDescriptor glass_probe_grid_bind_group = {
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
                        .type = WGPUBufferBindingType_ReadOnlyStorage,
                        .hasDynamicOffset = false,
                        .minBindingSize =
                            sizeof(ProbeReflectionUniform) * SSBO_CAPACITY,
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
                        .viewDimension = WGPUTextureViewDimension_CubeArray,
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

static const ShaderPipelineStateObject layout_glass_probe_grid = {
    .label = "Pipeline Bind Groups - Glass Probe Grid",
    .shader_path =
        "./backend/std_pipeline/modules/glass_probe_grid/"
        "glass_probe_grid.wgsl",
    .bind_groups_count = 2,
    .bind_groups = {&mvp_layout, &glass_probe_grid_bind_group},
    .bindings = {.mvp = &mvp_binding},
};

#endif
