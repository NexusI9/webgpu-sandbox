#ifndef _MBIN_H_
#define _MBIN_H_

#include <stdint.h>
#include <stdlib.h>

typedef float mbin_vertex_t;
typedef uint16_t mbin_index_t;
typedef uint8_t vec_dimension_t;

typedef struct {
  size_t count;
  mbin_vertex_t *data;
} VertexBuffer;

typedef struct {
  size_t count;
  mbin_index_t *data;
} IndexBuffer;

int write_buffer(const char *, void *, size_t, size_t);

#endif
