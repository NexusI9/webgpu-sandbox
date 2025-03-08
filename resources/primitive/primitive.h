#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include "../runtime/vertex.h"
#include <webgpu/webgpu.h>

typedef struct {

  vertex_attribute vertex;
  vertex_index index;

} primitive;

#endif
