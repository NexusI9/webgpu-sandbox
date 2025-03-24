#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include "../runtime/vertex.h"
#include <webgpu/webgpu.h>

typedef struct {

  VertexAttribute vertex;
  VertexIndex index;

} primitive;

#endif
