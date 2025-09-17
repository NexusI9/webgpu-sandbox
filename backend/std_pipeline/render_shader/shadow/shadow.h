#ifndef _PIPELINE_LAYOUT_SHADOW_H_
#define _PIPELINE_LAYOUT_SHADOW_H_

#include "runtime/pipeline/render.h"
#include "runtime/camera/camera.h"
#include "runtime/mesh/mesh.h"
#include "runtime/viewport/viewport.h"

#include "../commons.h"
#include "runtime/light/shadow_map/shadow_map.h"

#include <webgpu/webgpu.h>

static const PipelineBindingMVP shadow_mvp = {
    .group = 0,
    .projection = 0,
    .view = 1,
    .model = 2,
};

static const RenderPipelineStateObject layout_shadow = {
    .label = "Pipeline Bind Groups - Shadow",
    .shader_path = "./backend/std_pipeline/render_shader/"
                   "shadow/shadow.wgsl",
    .bind_groups_count = 1,
    .bind_groups = {&mp_layout},
    .pipeline_attributes =
        {
            .multisample_state =
                (WGPUMultisampleState){
                    .alphaToCoverageEnabled = false,
                    .mask = 0xFFFFFFFF,
                    .count = PipelineMultisampleCount_1x,
                },
            .stencil_state =
                (WGPUDepthStencilState){
                    .format = SHADOW_DEPTH_FORMAT,
                    .depthWriteEnabled = true,
                    .depthCompare = WGPUCompareFunction_Less,
                },

            /* need to set the cullback to FRONT for point light because
             * the light POV render is flipped on the X axis to match
             * the cubemap coordinates, such negative scaling lead to
             * set the cullback to front.*/
            .primitive_state =
                (WGPUPrimitiveState){
                    .frontFace = WGPUFrontFace_CCW,
                    .cullMode = WGPUCullMode_Front,
                    .topology = WGPUPrimitiveTopology_TriangleList,
                    .stripIndexFormat = WGPUIndexFormat_Undefined,
                },
        },
};

static const RenderPipelineStateObject layout_shadow_cullback = {
    .label = "Pipeline Bind Groups - Shadow Cullback",
    .shader_path =
        "./backend/std_pipeline/render_shader/shadow/shadow.wgsl",
    .bind_groups_count = 1,
    .bind_groups = {&mp_layout},
    .pipeline_attributes =
        {
            .multisample_state =
                (WGPUMultisampleState){
                    .alphaToCoverageEnabled = false,
                    .mask = 0xFFFFFFFF,
                    .count = PipelineMultisampleCount_1x,
                },
            .stencil_state =
                (WGPUDepthStencilState){
                    .format = SHADOW_DEPTH_FORMAT,
                    .depthWriteEnabled = true,
                    .depthCompare = WGPUCompareFunction_Less,
                },
            .primitive_state =
                (WGPUPrimitiveState){
                    .frontFace = WGPUFrontFace_CCW,
                    .cullMode = WGPUCullMode_Back,
                    .topology = WGPUPrimitiveTopology_TriangleList,
                    .stripIndexFormat = WGPUIndexFormat_Undefined,
                },
        },
};

#endif
