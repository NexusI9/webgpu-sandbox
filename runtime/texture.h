
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../utils/vector.h"
#include <cglm/cglm.h>
#include <stdint.h>
#include <webgpu/webgpu.h>

#define TEXTURE_CHANNELS_RGBA 4
#define TEXTURE_CHANNELS_RGB 3
#define TEXTURE_CHANNELS_R 1
#define TEXTURE_MIN_SIZE 64

typedef unsigned char *TextureData;

typedef struct {
  int width;
  int height;
  uint8_t channels;
  uint8_t value;
} TextureCreateDescriptor;

typedef struct {
  int width;
  int height;
  size_t size;
  TextureData data;
  uint8_t channels;
  uint8_t value;
} texture;

typedef struct {
  const texture *source;
  TextureData *destination;
  float thickness;
  float diffusion;

  struct {
    float x, y;
    float *value;
  } start;

  struct {
    float x, y;
    float *value;
  } end;

} TextureWriteLineDescriptor;

typedef struct {
  struct {
    ivec2 position;
    float *value;
  } a;

  struct {
    ivec2 position;
    float *value;
  } b;

  struct {
    ivec2 position;
    float *value;
  } c;

} TextureTriangleGradientDescriptor;

typedef struct {
  const texture *source;
  TextureData *destination;
  TextureTriangleGradientDescriptor *points;
  size_t length;
} TextureWriteTriangleGradientDescriptor;

void texture_create(texture *, const TextureCreateDescriptor *);
void texture_create_by_ref(unsigned char **, size_t *,
                           const TextureCreateDescriptor *);

void texture_fill(texture *, int);
void texture_write_pixel(texture *, int, vec2);
void texture_save(texture *, const char *);
void texture_free(texture *);
void texture_blur(const texture *, int, float, TextureData *);
void texture_write_line(const TextureWriteLineDescriptor *);
void texture_contrast(const texture *, float, TextureData *);
void texture_remap(const texture *, int, int, TextureData *);
void texture_write_triangle_gradient(
    const TextureWriteTriangleGradientDescriptor *);

void texture_read_pixel(const texture *, const ivec2, float *);

#endif
