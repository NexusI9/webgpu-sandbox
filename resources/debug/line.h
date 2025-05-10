#ifndef _LINE_H_
#define _LINE_H_

#include "../runtime/mesh.h"
#include <cglm/cglm.h>

#define LINE_MAX_POINTS 1024
#define LINE_THICKNESS 0.005
#define LINE_VERTEX_COUNT 4 // generate 8 (2*4) vertex per line
#define LINE_INDEX_COUNT 6 // generate 12 (2*6) index per line

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  const char *name;
} LineCreateDescriptor;

typedef struct {
  VertexAttribute *vertex;
  VertexIndex *index;
  vec3 *points;
  vec3 color;
} LineCreatePlaneDescriptor;

void line_create(mesh *, const LineCreateDescriptor *);
void line_add_point(mesh *, vec3, vec3, vec3);

#endif
