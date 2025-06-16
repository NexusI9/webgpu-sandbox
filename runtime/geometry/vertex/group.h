#ifndef _VERTEX_GROUP_H_
#define _VERTEX_GROUP_H_
#include "core.h"
#include "index.h"
#include <stddef.h>

#define VERTEX_GROUP_SUCCESS 0
#define VERTEX_GROUP_ALLOC_FAIL 1
#define VERTEX_GROUP_ERROR 2

typedef struct {
  vindex_t *entries;
  size_t length;
  size_t capacity;
} VertexGroup;



#endif
