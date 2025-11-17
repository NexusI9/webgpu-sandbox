#include "create.h"

#include <stdlib.h>

#include "core.h"
#include "stb/stb_image.h"
#include "stb/stb_image_resize2.h"
#include "backend/logger.h"
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
    logger_add(LoggerFlag_Error, "Could not create texture.");
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

  logger_add(LoggerFlag_Import, "texture: %s", desc->path);

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
    logger_add(LoggerFlag_Error, "Couldn't load texture from file.");
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
      logger_add(LoggerFlag_Warning, "Couldn't allocate resources for resize texture.");
    } else if (stbir_resize_uint8_srgb(data, width, height, 0, n_data,
                                       desc->width, desc->height, 0,
                                       (uint8_t)texture->channels) == NULL) {
      logger_add(LoggerFlag_Warning, "STBI resize texture fail.");
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

TextureStatus
texture_create_cubemap_from_file(Texture texture[TEXTURE_CUBE_LAYER],
                                 const TextureCreateCubeMapDescriptor *desc) {

  // put path in order
  const char *path_sort[TEXTURE_CUBE_LAYER] = {
      desc->path->right,  // +X
      desc->path->left,   // -X
      desc->path->top,    // +Y
      desc->path->bottom, // -Y
      desc->path->front,  // +Z
      desc->path->back,   // -Z
  };

  // load image to layer textures
  for (size_t i = 0; i < TEXTURE_CUBE_LAYER; i++) {
    const char *path = path_sort[i];

    if (texture_create_from_file(&texture[i],
                                 &(TextureCreateFileDescriptor){
                                     .channels = TextureChannel_Undefined,
                                     .height = desc->resolution,
                                     .width = desc->resolution,
                                     .flip = false,
                                     .path = path,
                                 }) == TextureStatus_Success) {

    } else {
      logger_add(LoggerFlag_Error, "Couldn't read cubemap texture.");
      return TextureStatus_FileError;
    }
  }

  return TextureStatus_Success;
}
