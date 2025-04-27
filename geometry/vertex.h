#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <cglm/cglm.h>
#include <stddef.h>

#define VERTEX_STRIDE 11
/*
  Cannonical structure of a vertex
 */
typedef struct {
  vec3 position;
  vec3 normal;
  vec3 color;
  vec2 uv;
} vertex;

typedef struct {
  float *data;
  size_t length;
} VertexAttribute;

typedef struct {
  uint16_t *data;
  size_t length;
} VertexIndex;

/*
  List of mesh vertex attributes and index
 */
typedef struct {

  float *position;
  float *normal;
  float *color;
  float *uv;

  uint16_t *index;
  size_t count;

} VertexList; // TODO: unsure about the naming..

void vertex_create(vertex *);
void vertex_list_create(VertexList *, size_t);
vertex vertex_from_array(float *);

#endif
