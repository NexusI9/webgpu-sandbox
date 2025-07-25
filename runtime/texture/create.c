#include "create.h"
#include "../../include/stb/stb_image.h"
#include "../utils/system.h"
#include "./write.h"
#include "core.h"
#include "string.h"
#include "webgpu/webgpu.h"

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
TextureStatus texture_create_from_file(Texture *texture, const char *path, bool flip) {
  // flip vertically so match wgpu coordinates
  stbi_set_flip_vertically_on_load(flip);

  VERBOSE_IMPORT("Importing texture: %s", path);
  
  int width, height, channels;
  texture_data data = stbi_load(path, &width, &height, &channels, 4);

  if (data == NULL) {
    VERBOSE_ERROR("Couldn't load texture from file.");
    texture->data = NULL;
    texture->width = 0;
    texture->height = 0;
    texture->channels = 0;
    texture->size = 0;
    return TextureStatus_FileError;
  }

  texture->size = width * height * 4;
  texture->data = data;
  texture->width = width;
  texture->height = height;
  texture->channels = 4;

  return TextureStatus_Success;
}
