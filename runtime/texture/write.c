#include "write.h"
#include "../utils/math.h"

/**
   Provide different operators to write value into a texture data
 */
void texture_write(unsigned char value, texture_data data,
                   TextureWriteMethod method) {

  switch (method) {
  case TextureWriteMethod_Replace:
    *data = clamp(value, 0, 255);
    break;

  case TextureWriteMethod_Add:
    *data = clamp(*data + value, 0, 255);
    break;

  case TextureWriteMethod_Mul:
    *data = clamp(*data * value, 0, 255);
    break;

  case TextureWriteMethod_Sub:
    *data = clamp(*data - value, 0, 255);
    break;

  case TextureWriteMethod_Div:
    *data = clamp(*data / value, 0, 255);
    break;
  };
}

void texture_write_pixel(Texture *texture, int value, vec2 coordinate,
                         TextureWriteMethod write_method) {

  int x = (int)coordinate[0];
  int y = (int)coordinate[1];

  if (x >= 0 && x < texture->width && y >= 0 && y < texture->height) {
    for (int c = 0; c < texture->channels; c++) {
      int offset = (y * texture->width + x) * texture->channels + c;
      texture_write(value, &texture->data[offset], write_method);
    }
  } else {
    // perror("Pixel coordinate is out of bound");
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
          texture_write((unsigned char)(blended * 255.0f), &pixel[c],
                        desc->write_method);
        }
      }
    }
  }
}

/**
   Create a faded dot between at least 3 points
 */
void texture_write_triangle_gradient(
    const TextureWriteTriangleGradientDescriptor *desc) {

  unsigned int width = desc->source->width;
  unsigned int height = desc->source->height;
  unsigned int channels = desc->source->channels;
  texture_data *out = desc->destination;

  for (size_t e = 0; e < desc->length; e++) {

    const ivec2 A = {
        desc->points[e].a.position[0],
        desc->points[e].a.position[1],
    };
    const float *value_a = desc->points[e].a.value;

    const ivec2 B = {
        desc->points[e].b.position[0],
        desc->points[e].b.position[1],
    };
    const float *value_b = desc->points[e].b.value;

    const ivec2 C = {
        desc->points[e].c.position[0],
        desc->points[e].c.position[1],
    };
    const float *value_c = desc->points[e].c.value;

    // triangle bounding box
    int minX = MIN(MIN(A[0], B[0]), C[0]);
    int minY = MIN(MIN(A[1], B[1]), C[1]);
    int maxX = MAX(MAX(A[0], B[0]), C[0]);
    int maxY = MAX(MAX(A[1], B[1]), C[1]);

    // scan global bounding box
    for (int y = minY; y < maxY; y++) {
      for (int x = minX; x < maxX; x++) {
        vec2 P = {(float)x + 0.5f, (float)y + 0.5f};

        // convert to float to get baycentric coordinates
        vec2 fA, fB, fC;
        ivec_to_vec(A, VectorLength_2, fA);
        ivec_to_vec(B, VectorLength_2, fB);
        ivec_to_vec(C, VectorLength_2, fC);

        // get baycentric coorindates
        float u, v, w;
        vec_baycentric(fA, fB, fC, P, VectorLength_2, &u, &v, &w);

        // narrow down to baycentric area
        if (u > 0 || v > 0 || w > 0) {
          // safe zone (within texture bound)
          if (x < 0 || x >= width || y < 0 || y >= height)
            continue;

          texture_data pixel = *desc->destination + (y * width + x) * channels;

          // printf("%p\n", pixel);

          for (int c = 0; c < channels; c++) {
            float v = value_a[c] * u + value_b[c] * v + value_c[c] * w;
            v = fminf(fmax(v, 0.0f), 1.0f);
            texture_write((unsigned char)(v * 255.0f), &pixel[c],
                          desc->write_method);
          }
        }
      }
    }
  }
}
