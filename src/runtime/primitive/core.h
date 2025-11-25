#ifndef _PRIMITIVE_CORE_H_
#define _PRIMITIVE_CORE_H_

#include <webgpu/webgpu.h>

#include "runtime/geometry/vertex/vertex.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/index.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/index.h"
#include "utils/defines.h"

typedef struct {

  VertexAttribute vertex;
  VertexIndex index;

} Primitive;

EXTERN_C_BEGIN

void primitive_destroy(Primitive*);

EXTERN_C_END
#endif
