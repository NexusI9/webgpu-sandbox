#include "read.h"

void texture_read_pixel(const Texture *source, const ivec2 coordinate,
                        float *pixel) {
  *pixel = source->data[(coordinate[1] * source->width + coordinate[0]) *
                        source->channels];
}
