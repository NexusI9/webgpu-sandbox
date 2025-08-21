#ifndef _PIPELINE_LAYOUT_SOLID_H_
#define _PIPELINE_LAYOUT_SOLID_H_

#include "../../core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/viewport/viewport.h"

#include <webgpu/webgpu.h>

static const PipelineLayoutDescriptor layout_solid = {
    .label = "Pipeline Bind Groups - Solid",
    .shader_path =
        "./backend/renderer/scene/std_pipeline/modules/solid/solid.wgsl",
    .bind_groups_count = 1,
    .bind_groups =
        (WGPUBindGroupLayoutDescriptor[]){
            {
                // Group 0: Camera, Viewport, Mesh
                .label = "Group 0 - Camera, Viewport, Mesh",
                .entryCount = 3,
                .entries =
                    (WGPUBindGroupLayoutEntry[]){
                        {
                            // uViewport
                            .binding = 0,
                            .visibility = WGPUShaderStage_Vertex,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(ViewportUniform),
                                },
                        },
                        {
                            // uCamera
                            .binding = 1,
                            .visibility = WGPUShaderStage_Fragment |
                                          WGPUShaderStage_Vertex,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(CameraUniform),
                                },
                        },
                        {
                            // uMesh
                            .binding = 2,
                            .visibility = WGPUShaderStage_Fragment |
                                          WGPUShaderStage_Vertex,
                            .buffer =
                                (WGPUBufferBindingLayout){
                                    .type = WGPUBufferBindingType_Uniform,
                                    .hasDynamicOffset = false,
                                    .minBindingSize = sizeof(MeshUniform),
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
