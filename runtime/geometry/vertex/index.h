#ifndef _VERTEX_INDEX_H_
#define _VERTEX_INDEX_H_

#include "core.h"

/*
  If change is required, make sure to adjust the meshbin file indexes as
  they are u32 based as well.
 */
typedef uint32_t vindex_t;

typedef struct {
  vindex_t *entries;
  size_t length;
  size_t capacity;
  WGPUBuffer buffer;
} VertexIndex;

void vertex_index_print(VertexIndex *);
int vertex_index_copy(VertexIndex *, VertexIndex *);
void vertex_index_destroy(VertexIndex *);

int vertex_index_create(VertexIndex *, size_t, WGPUBuffer);
int vertex_index_insert(VertexIndex *, vindex_t *, size_t);

#endif
