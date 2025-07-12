#ifndef _TEXTURE_WRITE_H_
#define _TEXTURE_WRITE_H_

#include "core.h"
#include <stdint.h>

typedef enum {
  TextureWriteMethod_Replace,
  TextureWriteMethod_Add,
  TextureWriteMethod_Mul,
  TextureWriteMethod_Sub,
  TextureWriteMethod_Div,
} TextureWriteMethod;

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

void texture_write(unsigned char, texture_data, TextureWriteMethod);

void texture_write_alpha(Texture *, uint8_t);

void texture_write_triangle_gradient(
    const TextureWriteTriangleGradientDescriptor *);
void texture_write_line(const TextureWriteLineDescriptor *);
void texture_write_pixel(Texture *, int, vec2, TextureWriteMethod);

#endif
