#ifndef _TEXTURE_CORE_H_
#define _TEXTURE_CORE_H_

#include <cglm/cglm.h>
#include <stdint.h>
#include <webgpu/webgpu.h>
#include <stddef.h>

#include "utils/vector/vector.h"

#define TEXTURE_MIN_SIZE 64
#define TEXTURE_FORMAT_OFFSCREEN_DEFAULT WGPUTextureFormat_RGBA8Unorm
#define TEXTURE_FORMAT_ONSCREEN_DEFAULT WGPUTextureFormat_BGRA8Unorm

typedef enum {
  TextureChannel_Undefined = 0,
  TextureChannel_R = 1,
  TextureChannel_RGB = 3,
  TextureChannel_RGBA = 4,
} TextureChannel;

typedef enum {
  TextureResolution_Undefined = 0,
  TextureResolution_16 = 16,
  TextureResolution_32 = 32,
  TextureResolution_64 = 64,
  TextureResolution_128 = 128,
  TextureResolution_256 = 256,
  TextureResolution_512 = 512,
  TextureResolution_1024 = 1024,
  TextureResolution_2048 = 2048,
} TextureResolution;

typedef TextureResolution texture_size_t[2];

typedef enum {
  TextureStatus_Success,
  TextureStatus_AllocFail,
  TextureStatus_FileError,
} TextureStatus;

typedef unsigned char *texture_data;

typedef struct {
  TextureResolution width;
  TextureResolution height;
  size_t size;
  texture_data data;
  TextureChannel channels;
  uint8_t value;
} Texture;

void texture_save(Texture *, const char *);
void texture_free(Texture *);

#endif
