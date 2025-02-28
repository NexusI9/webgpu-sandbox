#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include <webgpu/webgpu.h>

typedef struct {
  const float *data;
  uint16_t length;
} vertex;

typedef struct {
  const uint16_t *data;
  uint16_t length;
} index;

typedef struct {

  vertex vertex;
  index index;

} primitive;

#endif
