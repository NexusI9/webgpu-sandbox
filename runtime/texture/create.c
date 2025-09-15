#include "create.h"

#include <stdlib.h>

#include "include/stb/stb_image.h"
#include "backend/buffer.h"
#include "include/stb/stb_image_resize2.h"
#include "utils/system.h"
#include "core.h"
#include "webgpu/webgpu.h"
#include "write.h"

void texture_create(Texture *texture, const TextureCreateDescriptor *desc) {

  // if (texture->data)
  // texture_free(texture);

  texture->width = desc->width;
  texture->height = desc->height;
  texture->channels = desc->channels;
  texture->size = texture->width * texture->height * texture->channels;
  texture->data =
      (void *)calloc(texture->width * texture->height, texture->channels);

  if (texture->data == NULL) {
    VERBOSE_ERROR("Could not create texture.");
    return;
  }

  if (desc->value != NULL)
    texture_write_fill(texture, desc->value);
}

/**
   load picture from file
 */
TextureStatus
texture_create_from_file(Texture *texture,
                         const TextureCreateFileDescriptor *desc) {

  VERBOSE_IMPORT("texture: %s", desc->path);

  if (desc->channels == TextureChannel_Undefined)
    texture->channels = TextureChannel_RGBA;
  else
    texture->channels = desc->channels;

  // flip vertically so match wgpu coordinates
  stbi_set_flip_vertically_on_load(desc->flip);

  int width, height, channels;
  texture_data data =
      stbi_load(desc->path, &width, &height, &channels, texture->channels);

  if (data == NULL) {
    VERBOSE_ERROR("Couldn't load texture from file.");
    texture->data = NULL;
    texture->width = 0;
    texture->height = 0;
    texture->channels = 0;
    texture->size = 0;
    return TextureStatus_FileError;
  }


  // resize if width and height are provided in the descriptor
  if ((desc->width > TextureResolution_Undefined && width > desc->width) ||
      (desc->height > TextureResolution_Undefined && height > desc->height)) {

    unsigned char *n_data =
        malloc(desc->width * desc->height * texture->channels);

    if (n_data == NULL) {
      VERBOSE_WARNING("Couldn't allocate resources for resize texture.");
    } else if (stbir_resize_uint8_srgb(data, width, height, 0, n_data,
                                       desc->width, desc->height, 0,
                                       (uint8_t)texture->channels) == NULL) {
      VERBOSE_WARNING("STBI resize texture fail.");
    } else {

      // free old texture
      stbi_image_free(data);

      texture->width = desc->width;
      texture->height = desc->height;
      data = n_data;
    }
  } else {
    texture->width = width;
    texture->height = height;
  }

  texture->size = texture->width * texture->height * texture->channels;
  texture->data = data;

  return TextureStatus_Success;
}

static const int layer_count = 6;

TextureStatus
texture_create_cubemap_from_file(WGPUTexture *gpu_texture,
                                 const TextureCreateCubeMapDescriptor *desc) {

  *gpu_texture = wgpuDeviceCreateTexture(
      desc->device,
      &(WGPUTextureDescriptor){
          .dimension = WGPUTextureDimension_2D,
          .format = desc->format,
          .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
          .sampleCount = 1,
          .mipLevelCount = 1,
          .size =
              (WGPUExtent3D){
                  .width = desc->resolution,
                  .height = desc->resolution,
                  .depthOrArrayLayers = layer_count,
              },
      });

  // put path in order
  const char *path_sort[6] = {
      desc->path->right,  // +X
      desc->path->left,   // -X
      desc->path->top,    // +Y
      desc->path->bottom, // -Y
      desc->path->front,  // +Z
      desc->path->back,   // -Z
  };

  // load image to layer textures
  for (size_t i = 0; i < layer_count; i++) {
    const char *path = path_sort[i];
    
    Texture layer_texture;
    if (texture_create_from_file(&layer_texture,
                                 &(TextureCreateFileDescriptor){
                                     .channels = TextureChannel_Undefined,
                                     .height = desc->resolution,
                                     .width = desc->resolution,
                                     .flip = false,
                                     .path = path,
                                 }) == TextureStatus_Success) {

      // upload image to gpu and update relative layer texture
      buffer_create_texture_cube(
          &(CreateTextureCubeDescriptor){
              .texture = gpu_texture,
              .queue = desc->queue,
              .width = layer_texture.width,
              .height = layer_texture.height,
              .size = layer_texture.size,
              .data = layer_texture.data,
              .channels = layer_texture.channels,
              .format = desc->format,
              .layer = i,
          },
          BufferTextureMemory_Free);

    } else {
      VERBOSE_ERROR("Couldn't read cubemap texture.");
      return TextureStatus_FileError;
    }
  }

  return TextureStatus_Success;
}
