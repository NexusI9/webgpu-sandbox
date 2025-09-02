#ifndef _STD_TEXTURE_CORE_H_
#define _STD_TEXTURE_CORE_H_
#include <stdbool.h>
#include <webgpu/webgpu.h>

#define STD_TEXTURE_VIEW_COUNT 7

typedef enum {
  TextureViewType_Float,
  TextureViewType_Float2DArray,
  TextureViewType_FloatCube,
  TextureViewType_FloatCubeArray,
  TextureViewType_Depth,
  TextureViewType_DepthCubeArray,
  TextureViewType_Depth2DArray,
} TextureViewType;

typedef WGPUTextureView (*std_texture_view_create)(WGPUTexture *,
                                                   const WGPUDevice,
                                                   const WGPUQueue);

extern WGPUTextureView g_std_texture_view[STD_TEXTURE_VIEW_COUNT];
extern WGPUTexture g_std_texture[STD_TEXTURE_VIEW_COUNT];

/* Fallbacks Textures */
void scene_renderer_init_fallback_textures(const WGPUDevice, const WGPUQueue);
bool is_std_texture_view(const WGPUTextureView);
const WGPUTextureView std_texture_view(const TextureViewType);

#endif
