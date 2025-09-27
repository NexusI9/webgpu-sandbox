#ifndef _RENDER_PASS_TEXTURE_H_
#define _RENDER_PASS_TEXTURE_H_

#include <webgpu/webgpu.h>

#include "core.h"

typedef enum {
  RenderPassTextureStorage_Keep,
  RenderPassTextureStorage_Release,
} RenderPassTextureStorage;

void render_pass_texture_create_multisample(
    WGPUTexture *, WGPUTextureView *, const RenderPassTextureDescriptor *);

void render_pass_texture_create_monosample(WGPUTexture *, WGPUTextureView *,
                                           const RenderPassTextureDescriptor *);

void render_pass_texture_create_depth(WGPUTexture *, WGPUTextureView *,
                                      const RenderPassTextureDescriptor *);

void render_pass_list_create_shared_texture_color(
    RenderPassList *, const RenderPassTextureDescriptor *,
    const RenderPassTextureStorage, WGPUTexture *, WGPUTextureView *);

void render_pass_list_create_shared_texture_depth(
    RenderPassList *, const RenderPassTextureDescriptor *,
    const RenderPassTextureStorage, WGPUTexture *, WGPUTextureView *);

#endif
