#include "texture.h"
#include <stdlib.h>
#include <string.h>

void texture_create(texture *texture, const TextureCreateDescriptor *desc) {

  texture->size = desc->width * desc->height * desc->channels;
  texture->data = (void *)calloc(desc->width * desc->height, desc->channels);
  texture->width = desc->width;
  texture->height = desc->height;

  if (texture->data == NULL) {
    perror("Could not create texture");
    return;
  }

  if (desc->value != 0)
    memset(texture->data, desc->value, texture->size);
}

void texture_create_by_ref(unsigned char *data, size_t *size,
                           const TextureCreateDescriptor *desc) {

  *size = desc->width * desc->height * desc->channels;
  data = (void *)calloc(desc->width * desc->height, desc->channels);

  if (data == NULL) {
    perror("Could not create texture");
    return;
  }

  if (desc->value != 0)
    memset(data, desc->value, *size);
}

void texture_fill(texture *texture, int value) {
  memset(texture->data, value, texture->size);
}

void texture_write_pixel(texture *texture, int value, vec2 coordinate) {

  int x = (int)coordinate[0];
  int y = (int)coordinate[1];

  if (x >= 0 && x < texture->width && y >= 0 && y < texture->height) {
    int offset = (y * texture->width + x) * texture->channels;
    texture->data[offset] = value;     // R
    texture->data[offset + 1] = value; // G
    texture->data[offset + 2] = value; // B
  } else {
    perror("Pixel coordinate is out of bound");
  }
}

void texture_save(texture *texture, const char *path) {
    
}

void texture_free(texture *texture) { free(texture->data); }
