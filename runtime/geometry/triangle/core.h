#ifndef _TRIANGLE_CORE_H_
#define _TRIANGLE_CORE_H_

#include <stdint.h>
#include <cglm/types.h>

#include "../vertex/vertex.h"
#include "../runtime/geometry/vertex/core.h"
#include "../runtime/geometry/vertex/core.h"

typedef struct {
  Vertex a;
  Vertex b;
  Vertex c;
} Triangle;

typedef enum {
  TriangleStatus_RaycastSuccess,
  TriangleStatus_RaycastParallel,
  TriangleStatus_RaycastOutEdge1,
  TriangleStatus_RaycastOutEdge2,
  TriangleStatus_RaycastHitTooFar,
} TriangleStatus;

void triangle_random_points(Triangle *, uint16_t, vec3 *);
void triangle_normal(Triangle *, vec3);
TriangleStatus triangle_raycast(Triangle *, vec3, vec3, float, vec3);
void triangle_point_to_uv(Triangle *, vec3, vec2);
void triangle_center(Triangle *, vec3);

#endif
