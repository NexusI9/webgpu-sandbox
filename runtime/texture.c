#include "texture.h"
#include <stdlib.h>
#include <string.h>

void texture_create_fill(TextureCreateFillDescriptor *desc) {
  *desc->size = desc->width * desc->height * desc->channels;
  desc->data = (void *)calloc(desc->width * desc->height, desc->channels);
  if (desc->value != 0)
    memset(desc->data, desc->value, *desc->size);
}
