#ifndef _PIPELINE_LAYOUT_GLASS_H_
#define _PIPELINE_LAYOUT_GLASS_H_

#include "../../core.h"
#include "../commons.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"
#include <webgpu/webgpu.h>

typedef struct {
  float roughness;
  float frost_scale;
  float frost_strength;
  float _pad;
  color color;
  vec4 _pad1;
} __attribute__((aligned(16))) GlassUniform;

static const WGPUBindGroupLayoutDescriptor glass_env_bind_group = {
    // Group 1 (Environment map + sampler)
    .label = "Group 1 (Environment Map)",
    .entryCount = 3,
    .entries =
        (WGPUBindGroupLayoutEntry[]){
            {
                .binding = 0, // uGlassMaterial
                .visibility = WGPUShaderStage_Fragment,
                .buffer =
                    (WGPUBufferBindingLayout){
                        .type = WGPUBufferBindingType_Uniform,
                        .hasDynamicOffset = false,
                        .minBindingSize = sizeof(GlassUniform),
                    },
            },
            {
                .binding = 1, // env_map
                .visibility = WGPUShaderStage_Fragment,
                .texture =
                    (WGPUTextureBindingLayout){
                        .sampleType = WGPUTextureSampleType_Float,
                        .viewDimension = WGPUTextureViewDimension_Cube,
                        .multisampled = false,
                    },
            },
            {
                .binding = 2, // env_sampler
                .visibility = WGPUShaderStage_Fragment,
                .sampler =
                    (WGPUSamplerBindingLayout){
                        .type = WGPUSamplerBindingType_Filtering,
                    },
            },
        },
};

static const ShaderPipelineStateObject layout_glass = {
    .label = "Pipeline Bind Groups - Glass",
    .shader_path =
        "./backend/renderer/scene/std_pipeline/modules/glass/glass.wgsl",
    .bind_groups_count = 2,
    .bind_groups = {&mvp_layout, &glass_env_bind_group},
    .bindings = {.mvp = &mvp_binding},
};

#endif
