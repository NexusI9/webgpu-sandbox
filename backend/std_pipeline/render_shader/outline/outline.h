#ifndef _PIPELINE_LAYOUT_OUTLINE_H_
#define _PIPELINE_LAYOUT_OUTLINE_H_

#include "../commons.h"
#include <webgpu/webgpu.h>

static const RenderPipelineStateObject layout_outline = {
    .label = "Pipeline Bind Groups - Outline",
    .shader_path = "./backend/std_pipeline/render_shader/outline/outline.wgsl",
    .bind_groups_count = 1,
    .bind_groups = {&mvp_layout},
    .bindings = {.mvp = &mvp_binding},
    .pipeline_attributes = {
        .stencil_state = (WGPUDepthStencilState){
            .format = TEXTURE_FORMAT_DEPTH_STENCIL, // your depth-stencil
                                                    // format
            .depthWriteEnabled = false, // we don't want to overwrite depth
            .depthCompare =
                WGPUCompareFunction_Always, // disable depth test like
                                            // glDisable(GL_DEPTH_TEST)

            .stencilFront =
                (WGPUStencilFaceState){
                    .compare = WGPUCompareFunction_NotEqual, // GL_NOTEQUAL
                    .failOp = WGPUStencilOperation_Keep,
                    .depthFailOp = WGPUStencilOperation_Keep,
                    .passOp = WGPUStencilOperation_Keep},
            .stencilBack =
                (WGPUStencilFaceState){.compare = WGPUCompareFunction_NotEqual,
                                       .failOp = WGPUStencilOperation_Keep,
                                       .depthFailOp = WGPUStencilOperation_Keep,
                                       .passOp = WGPUStencilOperation_Keep},
            .stencilReadMask = 0xFF, // match GL mask
            .stencilWriteMask =
                0x00 // prevent writing to stencil (glStencilMask(0x00))
        }}};

#endif
