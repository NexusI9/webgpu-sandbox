#ifndef _TEXTURE_ATLAS_H_
#define _TEXTURE_ATLAS_H_

#include "runtime/texture/core.h"
#include <cglm/cglm.h>
#include <webgpu/webgpu.h>

#define TEXTURE_ATLAS_MAX_COL 64
#define TEXTURE_ATLAS_MAX_ROW 64

typedef struct {
  char *label;
  WGPUTexture texture;
  WGPUTextureView view;
  WGPUTextureFormat format;
  WGPUSampler sampler;
  ivec2 cell_size;
  ivec2 cell_count;
  int width;
  int height;
} TextureAtlas;

typedef struct {
  const char *label;
  const char *path;
  ivec2 cell_count;
  ivec2 cell_size;
  WGPUTextureFormat format;
  WGPUDevice device;
  WGPUQueue queue;
} TextureAtlasDescriptor;

#ifdef __cplusplus
extern "C" {
#endif

TextureStatus texture_atlas_create(TextureAtlas *,
                                   const TextureAtlasDescriptor *);
TextureStatus texture_atlas_cell_uv(TextureAtlas *, ivec2, vec2, vec2);
TextureStatus texture_atlas_destroy(TextureAtlas *);

static inline TextureStatus texture_atlas_cell_size(TextureAtlas *atlas,
                                                    ivec2 dest) {
  glm_ivec2_copy(atlas->cell_size, dest);
  return TextureStatus_Success;
}

#ifdef __cplusplus
}
#endif

#endif
