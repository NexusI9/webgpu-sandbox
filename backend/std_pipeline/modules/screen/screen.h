#ifndef _PIPELINE_LAYOUT_SCREEN_H_
#define _PIPELINE_LAYOUT_SCREEN_H_

#include "../../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include "../commons.h"
#include <webgpu/webgpu.h>

static const WGPUBindGroupLayoutDescriptor screen_layout_mesh = {
    // Group 0: Mesh
    .label = "Group 0 - Mesh",
    .entryCount = 1,
    .entries =
        (WGPUBindGroupLayoutEntry[]){
            {
                // uMesh
                .binding = 0,
                .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
                .buffer =
                    (WGPUBufferBindingLayout){
                        .type = WGPUBufferBindingType_ReadOnlyStorage,
                        .hasDynamicOffset = true,
                        .minBindingSize = sizeof(MeshUniform),
                    },
            },
        },
};

static const WGPUBindGroupLayoutDescriptor screen_layout_texture = {
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
                        .viewDimension = WGPUTextureViewDimension_2D,
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
};

static const ShaderPipelineStateObject layout_screen = {
    .label = "Pipeline Bind Groups - Screen",
    .shader_path = "./backend/std_pipeline/modules/"
                   "screen/screen.wgsl",
    .bind_groups_count = 2,
    .bind_groups = {&screen_layout_mesh, &screen_layout_texture},
    /* .pipeline_attributes =
         {
             .primitive_state =
                 (WGPUPrimitiveState){
                     .frontFace = WGPUFrontFace_CCW,
                     .cullMode = WGPUCullMode_None,
                     .topology = WGPUPrimitiveTopology_TriangleList,
                     .stripIndexFormat = WGPUIndexFormat_Undefined,
                 },
         },*/
    .bindings = {.mvp = &mvp_binding},
};

#endif
