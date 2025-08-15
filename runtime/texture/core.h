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

typedef enum {
  TextureSize_16 = 16,
  TextureSize_32 = 32,
  TextureSize_64 = 64,
  TextureSize_128 = 128,
  TextureSize_256 = 256,
  TextureSize_512 = 512,
  TextureSize_1024 = 1024,
  TextureSize_2048 = 2048,
} TextureSize;


typedef TextureSize texture_size_t[2];

typedef enum {
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
