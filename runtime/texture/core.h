#ifndef _TEXTURE_CORE_H_
#define _TEXTURE_CORE_H_

#include "../utils/vector/vector.h"
#include <cglm/cglm.h>
#include <stdint.h>
#include <webgpu/webgpu.h>

#define TEXTURE_CHANNELS_RGBA 4
#define TEXTURE_CHANNELS_RGB 3
#define TEXTURE_CHANNELS_R 1
#define TEXTURE_MIN_SIZE 64


typedef enum{
  TextureStatus_Success,
  TextureStatus_AllocFail,
  TextureStatus_FileError,
} TextureStatus;

typedef unsigned char *texture_data;

typedef struct {
  int width;
  int height;
  size_t size;
  texture_data data;
  uint8_t channels;
  uint8_t value;
} Texture;


void texture_save(Texture *, const char *);
void texture_free(Texture *);


#endif
