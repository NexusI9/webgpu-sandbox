
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <cglm/cglm.h>
#include <stdint.h>
#include <webgpu/webgpu.h>

#define TEXTURE_CHANNELS_RGBA 4
#define TEXTURE_CHANNELS_RGB 3
#define TEXTURE_MIN_SIZE 64

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
  unsigned char *data;
  uint8_t channels;
  uint8_t value;
} texture;

void texture_create(texture *, const TextureCreateDescriptor *);
void texture_create_by_ref(unsigned char **, size_t *,
                        const TextureCreateDescriptor *);

void texture_fill(texture *, int);
void texture_write_pixel(texture *, int, vec2);
void texture_save(texture *, const char *);
void texture_free(texture *);

#endif
