#ifndef _MBIN_H_
#define _MBIN_H_

#include <stdint.h>
#include <stdlib.h>

typedef uint16_t mbin_int;
typedef float mbin_vertex_t;
typedef mbin_int mbin_index_t;
typedef uint8_t vec_dimension_t;

typedef struct {
  size_t count;
  mbin_vertex_t *data;
} VertexBuffer;

typedef struct {
  size_t count;
  mbin_index_t *data;
} IndexBuffer;

typedef struct {
  mbin_int vertex_length;
  mbin_int vertex_size_type;
  mbin_int index_length;
  mbin_int index_size_type;
} MBINHeader;

typedef struct {
  mbin_vertex_t *vertex;
  mbin_index_t *index;
} MBINBody;

typedef struct {
  MBINHeader header;
  MBINBody body;
} MBINFile;

int write_buffer(const char *, void *, size_t, size_t);

#endif
