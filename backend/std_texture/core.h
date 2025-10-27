#ifndef _STD_TEXTURE_CORE_H_
#define _STD_TEXTURE_CORE_H_
#include <stdbool.h>
#include <webgpu/webgpu.h>

#define STD_TEXTURE_VIEW_COUNT 9

typedef enum {
  TextureViewType_Float,
  TextureViewType_FloatBlack,
  TextureViewType_FloatNormal,
  TextureViewType_Float2DArray,
  TextureViewType_FloatCube,
  TextureViewType_FloatCubeArray,
  TextureViewType_Depth,
  TextureViewType_DepthCubeArray,
  TextureViewType_Depth2DArray,
} TextureViewType;

typedef WGPUTextureView (*std_texture_view_create)(WGPUTexture *);

extern WGPUTextureView g_std_texture_view[STD_TEXTURE_VIEW_COUNT];
extern WGPUTexture g_std_texture[STD_TEXTURE_VIEW_COUNT];

/* Fallbacks Textures */
void standard_textures_init();
bool is_std_texture_view(const WGPUTextureView);
const WGPUTextureView std_texture_view(const TextureViewType);

#endif
