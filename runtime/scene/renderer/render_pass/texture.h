#ifndef _RENDER_PASS_TEXTURE_H_
#define _RENDER_PASS_TEXTURE_H_

#include <webgpu/webgpu.h>

#include "core.h"

void render_pass_create_color_multisample(RenderPass *);
void render_pass_create_color_monosample(RenderPass *);

void render_pass_create_multisampling_view(WGPUTexture *, WGPUTextureView *,
                                           const RenderPassTextureDescriptor *);

void render_pass_create_resolve_view(WGPUTexture *, WGPUTextureView *,
                                     const RenderPassTextureDescriptor *);

void render_pass_create_depth_view(WGPUTexture *, WGPUTextureView *,
                                   const RenderPassTextureDescriptor *);

#endif
