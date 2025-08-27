#ifndef _PIPELINE_LAYOUT_GRID_H_
#define _PIPELINE_LAYOUT_GRID_H_

#include "../../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/scene/editor/object/grid/grid.h"
#include "../runtime/viewport/viewport.h"

#include <webgpu/webgpu.h>

static const ShaderPipelineStateObject layout_grid = {
    .label = "Pipeline Bind Groups - Grid",
    .shader_path =
        "./backend/renderer/scene/std_pipeline/modules/grid/grid.wgsl",
    .bind_groups_count = 2,
    .bind_groups =
        (WGPUBindGroupLayoutDescriptor[]){
            {
                // Group 0 (Scene Data)
                .label = "Group 0 - Camera and Mesh",
                .entryCount = 3,
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
                                        sizeof(ViewportUniform),
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
                                        sizeof(CameraUniform),
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
                                        sizeof(MeshUniform),
                                },
                        },
                    },
            },
            {
                // Group 1 (Grid Data)
                .label = "Group 1 - Grid",
                .entryCount = 1,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            .binding = 0, // uGrid
                            .visibility = WGPUShaderStage_Fragment,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(GizmoGridUniform),
                                },
                        },
                    },
            },
        },
    .pipeline_attributes =
        {
            // double sided
            .primitive_state =
                (WGPUPrimitiveState){
                    .frontFace = WGPUFrontFace_CCW,
                    .cullMode = WGPUCullMode_None,
                    .topology = WGPUPrimitiveTopology_TriangleList,
                    .stripIndexFormat = WGPUIndexFormat_Undefined,
                },
            .stencil_state =
                (WGPUDepthStencilState){
                    .format = WGPUTextureFormat_Depth24Plus,
                    .depthWriteEnabled = false,
                    .depthCompare = WGPUCompareFunction_Less,
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
