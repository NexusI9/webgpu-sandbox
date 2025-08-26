#ifndef _PIPELINE_LAYOUT_GLASS_PROBE_H_
#define _PIPELINE_LAYOUT_GLASS_PROBE_H_

#include "../../core.h"
#include "../glass/glass.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/probe/reflection/grid.h"
#include "../runtime/viewport/viewport.h"

#include <webgpu/webgpu.h>

static const ShaderPipelineStateObject layout_glass_probe = {
    .label = "Pipeline Bind Groups - Glass Probe",
    .shader_path = "./backend/renderer/scene/std_pipeline/modules/glass_probe/"
                   "glass_probe.wgsl",
    .bind_groups_count = 2,
    .bind_groups =
        (WGPUBindGroupLayoutDescriptor[]){
            {
                // Group 0 (Uniforms)
                .label = "Group 0 (Scene + Glass Material Data)",
                .entryCount = 5,
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
                                        sizeof(ViewportUniform) * SSBO_CAPACITY,
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
                                        sizeof(CameraUniform) * SSBO_CAPACITY,
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
                                        sizeof(MeshUniform) * SSBO_CAPACITY,
                                },
                        },
                        {
                            .binding = 3, // uGlass
                            .visibility = WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(GlassUniform),
                                },
                        },
                        {
                            .binding = 4, // uProbeReflectionList
                            .visibility = WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize =
                                        sizeof(ProbeReflectionListUniform),
                                },
                        },
                    },
            },
            {
                // Group 1 (Reflection probes array + sampler)
                .label = "Group 1 (Reflection Probes)",
                .entryCount = 2,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            .binding = 0, // probe_reflection_maps
                            .visibility = WGPUShaderStage_Fragment,
                            .texture =
                                (WGPUTextureBindingLayout){
                                    .sampleType = WGPUTextureSampleType_Float,
                                    .viewDimension =
                                        WGPUTextureViewDimension_CubeArray,
                                    .multisampled = false,
                                },
                        },
                        {
                            .binding = 1, // probe_reflection_sampler
                            .visibility = WGPUShaderStage_Fragment,
                            .sampler =
                                (WGPUSamplerBindingLayout){
                                    .type = WGPUSamplerBindingType_Filtering,
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
