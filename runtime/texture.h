
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <stdint.h>
#include <webgpu/webgpu.h>
#define TEXTURE_CHANNELS_RGBA 4
#define TEXTURE_MIN_SIZE 64

typedef struct {
  int width;
  int height;
  size_t *size;
  void *data;
  uint8_t channels;
  uint8_t value;
} TextureCreateFillDescriptor;

void texture_create_fill(TextureCreateFillDescriptor *);

#endif
