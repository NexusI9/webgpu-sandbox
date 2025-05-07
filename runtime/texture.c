#include "texture.h"
#include "../utils/math.h"
#include <stdlib.h>
#include <string.h>

void texture_create(texture *texture, const TextureCreateDescriptor *desc) {

  // if (texture->data)
  // texture_free(texture);

  texture->width = desc->width;
  texture->height = desc->height;
  texture->channels = desc->channels;
  texture->size = texture->width * texture->height * texture->channels;
  texture->data =
      (void *)calloc(texture->width * texture->height, texture->channels);

  if (texture->data == NULL) {
    perror("Could not create texture");
    return;
  }

  if (desc->value != 0)
    memset(texture->data, desc->value, texture->size);
}

void texture_create_by_ref(unsigned char **data, size_t *size,
                           const TextureCreateDescriptor *desc) {

  *size = desc->width * desc->height * desc->channels;
  *data = (unsigned char *)calloc(desc->width * desc->height, desc->channels);

  if (data == NULL) {
    perror("Could not create texture");
    return;
  }

  if (desc->value != 0)
    memset(*data, desc->value, *size);
}

void texture_fill(texture *texture, int value) {
  memset(texture->data, value, texture->size);
}

void texture_write_pixel(texture *texture, int value, vec2 coordinate) {

  int x = (int)coordinate[0];
  int y = (int)coordinate[1];

  if (x >= 0 && x < texture->width && y >= 0 && y < texture->height) {
    for (int c = 0; c < texture->channels; c++) {
      int offset = (y * texture->width + x) * texture->channels + c;
      texture->data[offset] = value;
    }
  } else {
    // perror("Pixel coordinate is out of bound");
  }
}

void texture_save(texture *texture, const char *path) {}

void texture_free(texture *texture) { free(texture->data); }

/**
   Blur algorithm
   https://stackoverflow.com/questions/21418892/understanding-super-fast-blur-algorithm
   https://developer.apple.com/documentation/accelerate/blurring-an-image
 */
void texture_blur(const texture *src, int kernel_size, float sigma,
                  unsigned char **dest) {

  // texture attributes
  int w = src->width;
  int h = src->height;
  int channels = src->channels;
  unsigned char *in = src->data;
  unsigned char *out = *dest;

  // blur attributes
  int k = kernel_size / 2;
  float kernel[kernel_size][kernel_size];
  float kernel_sum = 0.0f;

  // calculate gaussian kernel
  for (int i = -k; i <= k; i++) {
    for (int j = -k; j <= k; j++) {
      kernel[i + k][j + k] = exp(-(i * i + j * j) / (2.0f * sigma * sigma));
      kernel_sum += kernel[i + k][j + k];
    }
  }

  // normalize kernel
  for (int i = 0; i < kernel_size; i++) {
    for (int j = 0; j < kernel_size; j++) {
      kernel[i][j] /= kernel_sum;
    }
  }

  // apply blur to each channels
  for (int y = k; y < h - k; y++) {
    for (int x = k; x < w - k; x++) {
      for (int c = 0; c < channels; c++) {
        float blurred_pixel = 0.0f;
        for (int i = -k; i <= k; i++) {
          for (int j = -k; j <= k; j++) {
            int ny = y + i; // y neighbors
            int nx = x + j; // x neighbors
            if (ny >= 0 && ny < h && nx >= 0 && nx < w) {
              int in_index = (ny * w + nx) * channels + c;
              blurred_pixel += (float)in[in_index] * kernel[i + k][j + k];
            }
          }
        }
        int out_index = (y * w + x) * channels + c;
        out[out_index] = (unsigned char)(blurred_pixel + 0.5f);
      }
    }
  }

  // handle boundaries
  for (int y = 0; y < k; y++) {
    for (int x = 0; x < w; x++) {
      for (int c = 0; c < channels; c++) {
        out[(y * w + x) * channels + c] = in[(y * w + x) * channels + c];
      }
    }
  }

  for (int y = h - k; y < h; y++) {
    for (int x = 0; x < w; x++) {
      for (int c = 0; c < channels; c++) {
        out[(y * w + x) * channels + c] = in[(y * w + x) * channels + c];
      }
    }
  }

  for (int y = k; y < h - k; y++) {
    for (int x = 0; x < k; x++) {
      for (int c = 0; c < channels; c++) {
        out[(y * w + x) * channels + c] = in[(y * w + x) * channels + c];
      }
    }
  }

  for (int y = k; y < h - k; y++) {
    for (int x = w - k; x < w; x++) {
      for (int c = 0; c < channels; c++) {
        out[(y * w + x) * channels + c] = in[(y * w + x) * channels + c];
      }
    }
  }
}

/**

 */
void texture_write_line(const TextureWriteLineDescriptor *desc) {

  uint8_t channels = desc->source->channels;
  int thickness = desc->thickness;
  int w = desc->source->width;
  int h = desc->source->height;
  float sigma = desc->diffusion;
  unsigned char **out = desc->destination;

  float x0 = desc->start.x;
  float y0 = desc->start.y;
  float *c0 = desc->start.value;

  float x1 = desc->end.x;
  float y1 = desc->end.y;
  float *c1 = desc->end.value;

  float dx = x1 - x0;
  float dy = y1 - y0;

  // TODO: check Bresenham approach (altough faster, harder to interpolate color
  // with it though)

  int steps = (int)(sqrt(dx * dx + dy * dy));

  for (int s = 0; s <= steps; ++s) {

    float t = (float)s / steps;
    float x = x0 + t * dx;
    float y = y0 + t * dy;

    float color[3];
    for (int c = 0; c < channels; c++)
      color[c] = (1 - t) * c0[c] + t * c1[c];

    int r = (int)(thickness / 2);
    for (int i = -r; i <= r; ++i) {
      for (int j = -r; j <= r; ++j) {
        int xi = (int)(x + i);
        int yj = (int)(y + j);

        if (xi < 0 || xi >= w || yj < 0 || yj >= h)
          continue;

        float dist = sqrt(i * i + j * j);
        float weight = expf(-(dist * dist) / (2 * sigma * sigma));
        unsigned char *pixel = *out + (yj * w + xi) * channels;
        for (int c = 0; c < channels; c++) {
          float orig = (float)pixel[c] / 255.0f;
          float blended = (1 - weight) * orig + weight * color[c];
          pixel[c] = (unsigned char)(blended * 255.0f);
        }
      }
    }
  }
}

void texture_contrast(const texture *source, float contrast,
                      TextureData *destination) {

  int w = source->width;
  int h = source->height;
  int channels = source->channels;

  for (int i = 0; i < w * h; ++i) {
    TextureData pixel = *destination + i * channels;

    for (int c = 0; c < MIN(channels, 3); c++) {
      float v = pixel[c] / 255.0f;
      // v = (v - 0.5f) * contrast + 0.5f;
      // v = fminf(fmaxf(v, 0.0f), 1.0f);
      v = 1.0f / (1.0f + expf(-contrast * (v - 0.5f)));
      pixel[c] = (unsigned char)(v * 255.0f);
    }
  }
}
/**
 */
void texture_remap(const texture *source, int min, int max,
                   TextureData *destination) {

  int w = source->width;
  int h = source->height;
  int channels = source->channels;

  float old_min = FLT_MAX;
  float old_max = -FLT_MAX;

  // get min / max value
  for (int i = 0; i < w * h; ++i) {
    TextureData pixel = source->data + i * channels;
    for (int c = 0; c < MIN(channels, 3); c++) {
      float v = pixel[c] / 255.0f;
      old_min = MIN(old_min, v);
      old_max = MAX(old_max, v);
    }
  }

  float old_range = old_max - old_min;

  if (old_range == 0.0f)
    old_range = 1.0f;

  float new_range = max - min;

  // remap
  for (int i = 0; i < w * h; ++i) {
    TextureData source_pixel = source->data + i * channels;
    TextureData dst_pixel = *destination + i * channels;

    for (int c = 0; c < MIN(channels, 3); ++c) {
      float v = source_pixel[c] / 255.0f;
      float old_v = v;
      v = ((v - old_min) / old_range) * new_range + min;
      v = fminf(fmaxf(v, 0.0f), 1.0f);
      dst_pixel[c] = (unsigned char)(v * 255.0f);
    }
  }
}
