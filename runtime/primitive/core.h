#ifndef _PRIMITIVE_CORE_H_
#define _PRIMITIVE_CORE_H_

#include "../geometry/vertex/vertex.h"
#include <webgpu/webgpu.h>

typedef struct {

  VertexAttribute vertex;
  VertexIndex index;

} Primitive;

void primitive_destroy(Primitive*);

#endif
