#include "atlas.h"
#include "runtime/texture/core.h"

#include "backend/buffer.h"
#include "runtime/texture/create.h"
#include "utils/system.h"
#include "webgpu/webgpu.h"

TextureStatus texture_atlas_create(TextureAtlas *atlas,
                                   const TextureAtlasDescriptor *desc) {

  if (desc->cell_count[0] > TEXTURE_ATLAS_MAX_ROW ||
      desc->cell_count[1] > TEXTURE_ATLAS_MAX_COL) {
    VERBOSE_ERROR("Attempting to set a cell count out of maximum allowed cell "
           "count: [%d,%d], trying to set [%d, %d].",
           TEXTURE_ATLAS_MAX_ROW, TEXTURE_ATLAS_MAX_COL, desc->cell_count[0],
           desc->cell_count[1]);
    return TextureStatus_CellOutOfBound;
  }

  Texture texture;
  TextureStatus create =
      texture_create_from_file(&texture, &(TextureCreateFileDescriptor){
                                             .channels = TextureChannel_RGBA,
                                             .flip = false,
                                             .height = TEXTURE_HEIGHT_AUTO,
                                             .width = TEXTURE_WIDTH_AUTO,
                                             .path = desc->path,
                                         });

  if (create != TextureStatus_Success) {
    VERBOSE_ERROR("Couldn't create texture atlas %s.", desc->label);
    return create;
  }

  if (desc->label)
    atlas->label = strdup(desc->label);

  buffer_create_texture(&atlas->texture, &atlas->view,
                        &(CreateTextureDescriptor){
                            .channels = texture.channels,
                            .data = texture.data,
                            .device = desc->device,
                            .format = desc->format,
                            .height = texture.height,
                            .width = texture.width,
                            .queue = desc->queue,
                            .size = texture.size,
                        },
                        BufferTextureMemory_Free);

  // atlas->sampler = wgpuDeviceCreateSampler(
  //     desc->device, &(WGPUSamplerDescriptor){
  //                       .label = "Atlas Sampler",
  //                       .addressModeU = WGPUAddressMode_ClampToEdge,
  //                       .addressModeV = WGPUAddressMode_ClampToEdge,
  //                       .addressModeW = WGPUAddressMode_ClampToEdge,
  //                       .compare = WGPUCompareFunction_Undefined,
  //                       .magFilter = WGPUFilterMode_Linear,
  //                       .minFilter = WGPUFilterMode_Linear,
  //                   });

  atlas->width = texture.width;
  atlas->height = texture.height;
  glm_ivec2_copy((int *)desc->cell_count, atlas->cell_count);
  glm_ivec2_copy((int *)desc->cell_size, atlas->cell_size);

  return TextureStatus_Success;
}

TextureStatus texture_atlas_cell_uv(TextureAtlas *atlas, ivec2 cell, vec2 uv0,
                                    vec2 uv1) {

  if (cell[0] > atlas->cell_count[0] || cell[1] > atlas->cell_count[1]) {
    printf("Attempting to reach a cell out of texture atlas %s. Maximum cell "
           "count: [%d,%d], trying to reach [%d, %d].",
           atlas->label, atlas->cell_count[0], atlas->cell_count[1], cell[0],
           cell[1]);
    return TextureStatus_CellOutOfBound;
  }

  ivec2 dimension = {atlas->width, atlas->height};

  for (int i = 0; i < 2; i++) {
    uv0[i] = (float)cell[i] * atlas->cell_size[i] / dimension[i];
    uv1[i] = (float)(cell[i] + 1) * atlas->cell_size[i] / dimension[i];
  }

  return TextureStatus_Success;
}

TextureStatus texture_atlas_destroy(TextureAtlas *atlas) {

  if (atlas->label) {
    free(atlas->label);
    atlas->label = NULL;
  }

  if (atlas->sampler)
    wgpuSamplerRelease(atlas->sampler);

  if (atlas->texture)
    wgpuTextureRelease(atlas->texture);

  if (atlas->view)
    wgpuTextureViewRelease(atlas->view);

  return TextureStatus_Success;
}
