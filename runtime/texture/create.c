#include "create.h"
#include "string.h"
#include "../../include/stb/stb_image.h"
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
    perror("Could not create texture");
    return;
  }

  if (desc->value != 0)
    memset(texture->data, desc->value, texture->size);
}


/**
   Create a texture from refenreced data
 */
void texture_create_from_ref(unsigned char **data, size_t *size,
                             const TextureCreateDescriptor *desc) {

  *size = desc->width * desc->height * desc->channels;
  *data = (unsigned char *)calloc(desc->width * desc->height, desc->channels);

  if (data == NULL) {
    perror("Could not create texture");
    return;
  }

  if (desc->value != 0)
    memset(*data, desc->value, *size);
}


/**
   load picture from file
 */
int texture_create_from_file(Texture *texture, const char *path, bool flip) {
  // flip vertically so match wgpu coordinates
  stbi_set_flip_vertically_on_load(flip);

  int width, height, channels;
  texture_data data = stbi_load(path, &width, &height, &channels, 4);

  if (data == NULL) {
    perror("Couldn't load texture from file\n");
    texture->data = NULL;
    texture->width = 0;
    texture->height = 0;
    texture->channels = 0;
    texture->size = 0;
    return TEXTURE_FILE_ERROR;
  }

  texture->size = width * height * 4;
  texture->data = data;
  texture->width = width;
  texture->height = height;
  texture->channels = 4;

  return TEXTURE_SUCCESS;
}
