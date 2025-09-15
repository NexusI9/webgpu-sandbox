#ifndef _LINE_CORE_H_
#define _LINE_CORE_H_

#include <cglm/cglm.h>
#include <webgpu/webgpu.h>
#include <cglm/types.h>
#include <stddef.h>

#include "runtime/mesh/mesh.h"
#include "runtime/geometry/vertex/vertex.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/index.h"
#include "runtime/mesh/core.h"

#define LINE_MAX_POINTS 1024
#define LINE_THICKNESS 0.005

// generate 8 (2*4) vertex per line
#define LINE_VERTEX_COUNT 4

// generate 12 (2*6) index per line
#define LINE_INDEX_COUNT 6

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  const char *name;
} LineCreateDescriptor;

typedef struct {
  VertexAttribute *vertex;
  VertexIndex *index;
  vec3 *points;
  vec3 color;
} LineCreatePlaneDescriptor;

typedef struct {

  vindex_t index;
} LineAnchor;

void line_create(Mesh *, const LineCreateDescriptor *);
void line_add_point(vec3, vec3, vec3, VertexAttribute *, VertexIndex *);
void line_update_buffer(Mesh *);
void line_set_vertex(const vec3, const vec3, const vec3, const vec2,
                     const size_t, float *);

#endif
