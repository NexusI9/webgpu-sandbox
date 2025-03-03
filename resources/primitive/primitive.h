#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include <webgpu/webgpu.h>

typedef struct {
  const float *data;
  uint16_t length;
} vertex_attribute;

typedef struct {
  const uint16_t *data;
  uint16_t length;
} vertex_index;

typedef struct {

  vertex_attribute vertex;
  vertex_index index;

} primitive;

#endif
