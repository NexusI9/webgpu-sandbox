#ifndef _VERTEX_LIST_H_
#define _VERTEX_LIST_H_

#include "core.h"
#include "index.h"

/*
  List of mesh vertex attributes and index
 */
typedef struct {

  float *position;
  float *normal;
  float *color;
  float *uv;

  vindex_t *index;
  size_t count;

} VertexList; // TODO: unsure about the naming..

void vertex_list_create(VertexList *, size_t);

#endif
