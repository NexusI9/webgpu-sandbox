#ifndef _TEXTURE_CREATE_H_
#define _TEXTURE_CREATE_H_

#include <stdbool.h>
#include <stdint.h>

#include "core.h"
#include "utils/defines.h"
#include "webgpu/webgpu.h"

#define TEXTURE_CUBE_LAYER 6

typedef struct {
  const int width;
  const int height;
  const uint8_t channels;
  const uint8_t *value;
} TextureCreateDescriptor;

typedef struct {
  const TextureResolution width;
  const TextureResolution height;
  const uint8_t channels;
  const char *path;
  bool flip;
} TextureCreateFileDescriptor;

typedef struct {
  const char *right;
  const char *left;
  const char *top;
  const char *bottom;
  const char *front;
  const char *back;
} CubeMapPath;

typedef struct {
  const TextureResolution resolution;
  const WGPUTextureFormat format;

  const CubeMapPath *path;
} TextureCreateCubeMapDescriptor;

EXTERN_C_BEGIN

void texture_create(Texture *, const TextureCreateDescriptor *);
TextureStatus texture_create_from_file(Texture *,
                                       const TextureCreateFileDescriptor *);
TextureStatus
texture_create_cubemap_from_file(Texture[TEXTURE_CUBE_LAYER],
                                 const TextureCreateCubeMapDescriptor *);

EXTERN_C_END

#endif
