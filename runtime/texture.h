
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

#define TEXTURE_SUCCESS 0
#define TEXTURE_ALLOC_FAIL 1
#define TEXTURE_FILE_ERROR 2

typedef unsigned char *texture_data;

typedef enum {
  TextureWriteMethod_Replace,
  TextureWriteMethod_Add,
  TextureWriteMethod_Mul,
  TextureWriteMethod_Sub,
  TextureWriteMethod_Div,
} TextureWriteMethod;

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
  texture_data data;
  uint8_t channels;
  uint8_t value;
} Texture;

typedef struct {
  const Texture *source;
  texture_data *destination;
  float thickness;
  float diffusion;
  TextureWriteMethod write_method;

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
  const Texture *source;
  texture_data *destination;
  TextureTriangleGradientDescriptor *points;
  size_t length;
  TextureWriteMethod write_method;
} TextureWriteTriangleGradientDescriptor;

void texture_create(Texture *, const TextureCreateDescriptor *);
int texture_create_from_file(Texture *, const char *);
void texture_create_from_ref(unsigned char **, size_t *,
                           const TextureCreateDescriptor *);

void texture_fill(Texture *, int);
void texture_save(Texture *, const char *);
void texture_free(Texture *);
void texture_blur(const Texture *, int, float, texture_data *);
void texture_contrast(const Texture *, float, texture_data *);
void texture_remap(const Texture *, int, int, texture_data *);

void texture_write_triangle_gradient(
    const TextureWriteTriangleGradientDescriptor *);
void texture_write_line(const TextureWriteLineDescriptor *);
void texture_write_pixel(Texture *, int, vec2, TextureWriteMethod);

void texture_read_pixel(const Texture *, const ivec2, float *);

#endif
