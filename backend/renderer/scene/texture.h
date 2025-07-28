#ifndef _SCENE_RENDERER_TEXTURE_H_
#define _SCENE_RENDERER_TEXTURE_H_

#include "../runtime/texture/texture.h"
#include "core.h"

typedef struct {
  int width;
  int height;
  PipelineMultisampleCount multisample;
  const WGPUDevice *device;
} SceneRendererTextureDescriptor;

// textures initializer called in the scene_renderer_create (./core.c)
void scene_renderer_init_fallback_textures(SceneRenderer *);

void scene_renderer_init_render_textures(SceneRenderer *);

WGPUSwapChain scene_renderer_create_swapchain(const SceneRenderer *);

#endif
