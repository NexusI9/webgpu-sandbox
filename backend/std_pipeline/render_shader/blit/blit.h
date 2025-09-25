#ifndef _PIPELINE_LAYOUT_BLIT_H_
#define _PIPELINE_LAYOUT_BLIT_H_

#include "../commons.h"
#include <webgpu/webgpu.h>

static const WGPUBindGroupLayoutDescriptor blit_bind_group_layout = {
    .label = "Group 0 - Blit Texture",
    .entryCount = 2,
    .entries =
        (WGPUBindGroupLayoutEntry[]){
            {
                .binding = 0, // source texture
                .visibility = WGPUShaderStage_Fragment,
                .texture =
                    (WGPUTextureBindingLayout){
                        .sampleType = WGPUTextureSampleType_Float,
                        .viewDimension = WGPUTextureViewDimension_2D,
                        .multisampled = false,
                    },
            },
            {
                .binding = 1, // sampler
                .visibility = WGPUShaderStage_Fragment,
                .sampler =
                    (WGPUSamplerBindingLayout){
                        .type = WGPUSamplerBindingType_Filtering,
                    },
            },
        },
};

static const RenderPipelineStateObject layout_blit = {
    .label = "Pipeline Blit / Fullscreen Pass",
    .shader_path = "./backend/std_pipeline/render_shader/blit/blit.wgsl",
    .bind_groups_count = 1,
    .bind_groups = {&blit_bind_group_layout},
    .pipeline_attributes =
        {
            .primitive_state =
                (WGPUPrimitiveState){
                    .topology = WGPUPrimitiveTopology_TriangleList,
                    .frontFace = WGPUFrontFace_CCW,
                    .cullMode = WGPUCullMode_None, // no cullmode
                    .stripIndexFormat = WGPUIndexFormat_Undefined,
                },
            .stencil_state =
                (WGPUDepthStencilState){
                    .format = WGPUTextureFormat_Undefined, // no stencil & depth
                    .depthWriteEnabled = false,
                    .depthCompare = WGPUCompareFunction_Always,
                },
            .multisample_state =
                (WGPUMultisampleState){
                    .count = 1,
                    .mask = ~0u,
                    .alphaToCoverageEnabled = false,
                },
            .vertex_state =
                (WGPUVertexState){
                    .bufferCount = 0,
                    .buffers = NULL,
                    .entryPoint = "vs_main",
                    .module = RENDER_PIPELINE_SET_KEEP_MODULE,
                },
        },
    .bindings = {0},
};

#endif
