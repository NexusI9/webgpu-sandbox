#ifndef _TEXTURE_CREATE_H_
#define _TEXTURE_CREATE_H_

#include "core.h"

typedef struct {
  int width;
  int height;
  uint8_t channels;
  uint8_t* value;
} TextureCreateDescriptor;

void texture_create(Texture *, const TextureCreateDescriptor *);

int texture_create_from_file(Texture *, const char *, bool flip);
void texture_create_from_ref(unsigned char **, size_t *,
                             const TextureCreateDescriptor *);

#endif
