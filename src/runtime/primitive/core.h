#ifndef _PRIMITIVE_CORE_H_
#define _PRIMITIVE_CORE_H_

#include <webgpu/webgpu.h>

#include "runtime/geometry/vertex/vertex.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/index.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/index.h"

typedef struct {

  VertexAttribute vertex;
  VertexIndex index;

} Primitive;

void primitive_destroy(Primitive*);

#endif
