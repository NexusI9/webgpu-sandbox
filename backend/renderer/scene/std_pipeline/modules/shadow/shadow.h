#ifndef _PIPELINE_LAYOUT_SHADOW_H_
#define _PIPELINE_LAYOUT_SHADOW_H_

#include "../../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include "../backend/renderer/scene/shadow_map/shadow_map.h"

#include <webgpu/webgpu.h>

static const ShaderPipelineStateObject layout_shadow = {
    .label = "Pipeline Bind Groups - Shadow",
    .shader_path =
        "./backend/renderer/scene/std_pipeline/modules/shadow/shadow.wgsl",
    .bind_groups_count = 1,
    .bind_groups =
        (WGPUBindGroupLayoutDescriptor[]){
            {
                // Group 0: view_projection + uModel
                .label = "Group 0 - Shadow Matrices",
                .entryCount = 2,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            // view_projection
                            .binding = 0,
                            .visibility = WGPUShaderStage_Vertex,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(mat4),
                                },
                        },
                        {
                            // uModel
                            .binding = 1,
                            .visibility = WGPUShaderStage_Vertex,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_ReadOnlyStorage,
                                    .hasDynamicOffset = true,
                                    .minBindingSize =
                                        sizeof(MeshUniform),
                                },
                        },
                    },
            },
        },
    .pipeline_attributes =
        {
            .multisample = PipelineMultisampleCount_1x,
            .stencil_state =
                (WGPUDepthStencilState){
                    .format = SHADOW_DEPTH_FORMAT,
                    .depthWriteEnabled = true,
                    .depthCompare = WGPUCompareFunction_Less,
                },

            /* need to set the cullback to FRONT for point light because the
             * light POV render is flipped on the X axis to match the cubemap
             * coordinates, such negative scaling lead to set the cullback to
             * front.*/
            .primitive_state =
                (WGPUPrimitiveState){
                    .frontFace = WGPUFrontFace_CCW,
                    .cullMode = WGPUCullMode_Front,
                    .topology = WGPUPrimitiveTopology_TriangleList,
                    .stripIndexFormat = WGPUIndexFormat_Undefined,
                },
        },
};

static const ShaderPipelineStateObject layout_shadow_cullback = {
    .label = "Pipeline Bind Groups - Shadow Cullback",
    .shader_path =
        "./backend/renderer/scene/std_pipeline/modules/shadow/shadow.wgsl",
    .bind_groups_count = 1,
    .bind_groups =
        (WGPUBindGroupLayoutDescriptor[]){
            {
                // Group 0: view_projection + uModel
                .label = "Group 0 - Shadow Matrices",
                .entryCount = 2,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            // view_projection
                            .binding = 0,
                            .visibility = WGPUShaderStage_Vertex,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(
                                        mat4), // adjust if using a struct
                                },
                        },
                        {
                            // uModel
                            .binding = 1,
                            .visibility = WGPUShaderStage_Vertex,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_ReadOnlyStorage,
                                    .hasDynamicOffset = true,
                                    .minBindingSize = sizeof(MeshUniform),
                                },
                        },
                    },
            },
        },
    .pipeline_attributes =
        {
            .multisample = PipelineMultisampleCount_1x,
            .stencil_state =
                (WGPUDepthStencilState){
                    .format = SHADOW_DEPTH_FORMAT,
                    .depthWriteEnabled = true,
                    .depthCompare = WGPUCompareFunction_Less,
                },

            /* need to set the cullback to FRONT for point light because the
             * light POV render is flipped on the X axis to match the cubemap
             * coordinates, such negative scaling lead to set the cullback to
             * front.*/
            .primitive_state =
                (WGPUPrimitiveState){
                    .frontFace = WGPUFrontFace_CCW,
                    .cullMode = WGPUCullMode_Back,
                    .topology = WGPUPrimitiveTopology_TriangleList,
                    .stripIndexFormat = WGPUIndexFormat_Undefined,
                },
        },
    .bindings =
        {
            .mvp =
                {
                    .group = 0,
                    .view = 0,
                    .projection = 0,
                    .model = 1,
                },
        },
};

#endif
