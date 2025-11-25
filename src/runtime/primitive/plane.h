#ifndef _PLANE_PRIMITIVE_H_
#define _PLANE_PRIMITIVE_H_

#include "core.h"

typedef enum {
  PrimitivePlaneUp_y,
  PrimitivePlaneUp_z,
} PrimitivePlaneUp;

EXTERN_C_BEGIN

Primitive primitive_plane();

EXTERN_C_END

#endif
