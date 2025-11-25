#ifndef _TEXTURE_SPRITE_SHEET_H_
#define _TEXTURE_SPRITE_SHEET_H_

#include "runtime/texture/core.h"
#include <cglm/cglm.h>
#include <webgpu/webgpu.h>

#define TEXTURE_SPRITE_SHEET_MAX_COL 64
#define TEXTURE_SPRITE_SHEET_MAX_ROW 64

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
} TextureSpriteSheet;

typedef struct {
  const char *label;
  const char *path;
  ivec2 cell_count;
  ivec2 cell_size;
  WGPUTextureFormat format;
} TextureSpriteSheetDescriptor;

EXTERN_C_BEGIN

TextureStatus texture_sprite_sheet_create(TextureSpriteSheet *,
                                          const TextureSpriteSheetDescriptor *);
TextureStatus texture_sprite_sheet_cell_uv(TextureSpriteSheet *, ivec2, vec2,
                                           vec2);
TextureStatus texture_sprite_sheet_destroy(TextureSpriteSheet *);

static inline TextureStatus
texture_sprite_sheet_cell_size(TextureSpriteSheet *atlas, ivec2 dest) {
  glm_ivec2_copy(atlas->cell_size, dest);
  return TextureStatus_Success;
}

EXTERN_C_END

#endif
