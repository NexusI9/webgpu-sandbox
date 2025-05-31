#ifndef _PLANE_PRIMITIVE_H_
#define _PLANE_PRIMITIVE_H_

#include "primitive.h"

typedef enum {
  PrimitivePlaneUp_y,
  PrimitivePlaneUp_z,
} PrimitivePlaneUp;

Primitive primitive_plane();

#endif
