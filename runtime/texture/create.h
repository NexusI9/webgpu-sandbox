#ifndef _TEXTURE_CREATE_H_
#define _TEXTURE_CREATE_H_

#include "core.h"

typedef struct {
  const int width;
  const int height;
  const uint8_t channels;
  const uint8_t* value;
} TextureCreateDescriptor;

void texture_create(Texture *, const TextureCreateDescriptor *);
TextureStatus texture_create_from_file(Texture *, const char *, bool flip);



#endif
