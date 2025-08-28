#ifndef _PIPELINE_LAYOUT_GLASS_PROBE_H_
#define _PIPELINE_LAYOUT_GLASS_PROBE_H_

#include "../../core.h"
#include "../glass/glass.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/probe/reflection/grid.h"
#include "../runtime/viewport/viewport.h"

#include <webgpu/webgpu.h>

static const WGPUBindGroupLayoutDescriptor glass_probe_bind_group = {
    // Group 1 (Reflection probes array + sampler)
    .label = "Group 1 (Reflection Probes)",
    .entryCount = 4,
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
                        .minBindingSize = sizeof(ProbeReflectionListUniform),
                    },
            },
            {
                .binding = 2, // probe_reflection_maps
                .visibility = WGPUShaderStage_Fragment,
                .texture =
                    (WGPUTextureBindingLayout){
                        .sampleType = WGPUTextureSampleType_Float,
                        .viewDimension = WGPUTextureViewDimension_CubeArray,
                        .multisampled = false,
                    },
            },
            {
                .binding = 3, // probe_reflection_sampler
                .visibility = WGPUShaderStage_Fragment,
                .sampler =
                    (WGPUSamplerBindingLayout){
                        .type = WGPUSamplerBindingType_Filtering,
                    },
            },
        },
};

static const ShaderPipelineStateObject layout_glass_probe = {
    .label = "Pipeline Bind Groups - Glass Probe",
    .shader_path = "./backend/renderer/scene/std_pipeline/modules/glass_probe/"
                   "glass_probe.wgsl",
    .bind_groups_count = 2,
    .bind_groups = {&mvp_layout, &glass_probe_bind_group},
    .bindings = {.mvp = &mvp_binding},
};

#endif
