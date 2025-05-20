#ifndef _MBIN_BUFFER_H_
#define _MBIN_BUFFER_H_

#include "mbin.h"
#include <stddef.h>

#define MBIN_BUFFER_DEFAULT_CAPACITY 1024
#define MBIN_BUFFER_SUCCESS 0
#define MBIN_BUFFER_ALLOC_FAILURE 1

typedef struct {
  size_t capacity;
  size_t length;
  mbin_vertex_t *entries;
} VertexBuffer;

typedef struct {
  size_t capacity;
  size_t length;
  mbin_index_t *entries;
} IndexBuffer;

int vertex_buffer_insert(VertexBuffer *, mbin_vertex_t *, size_t);
int index_buffer_insert(IndexBuffer *, mbin_index_t);

void vertex_buffer_free(VertexBuffer *);
void index_buffer_free(IndexBuffer *);

#endif
