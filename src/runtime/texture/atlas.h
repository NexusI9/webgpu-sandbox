#ifndef _TEXTURE_ATLAS_H_
#define _TEXTURE_ATLAS_H_

#include "backend/logger.h"
#include "runtime/texture/core.h"
#include <cglm/cglm.h>
#include <webgpu/webgpu.h>

#define TEXTURE_ATLAS_MAX_LAYER 12

typedef struct {
  const char *label;
  WGPUTexture texture;
  WGPUTextureView view[TEXTURE_ATLAS_MAX_LAYER];
} TextureAtlas;

typedef struct {
  const char *label;
  vec2 size;
  vec2 uv0, uv1;
} TextureAtlasRegion;

typedef struct {
  const char *label;

  struct {
    const char *paths[12];
    uint8_t count;
  } layers;

  const TextureResolution resolution;

} TextureAtlasDescriptor;

EXTERN_C_BEGIN

TextureStatus texture_atlas_create(TextureAtlas *,
                                   const TextureAtlasDescriptor *);

TextureStatus texture_atlas_destroy(TextureAtlas *);

static inline WGPUTextureView texture_atlas_layer_view(TextureAtlas *atlas,
                                                       const uint8_t layer) {

  if (layer > wgpuTextureGetDepthOrArrayLayers(atlas->texture)) {
    logger_add(LoggerFlag_Warning,
               "Attempting to access a layer index (%u) superior to the max "
               "layer count (%u) of texture atlas %s",
               layer, wgpuTextureGetDepthOrArrayLayers(atlas->texture),
               atlas->label);
    return NULL;
  }

  return atlas->view[layer];
}

EXTERN_C_END

#endif
