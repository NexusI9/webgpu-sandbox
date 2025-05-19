#ifndef _V_HASH_H_
#define _V_HASH_H_

#include "./mbin.h"
#include "vindex.h"
#include <stdbool.h>

typedef mbin_vertex_t *vhash_value_t;

typedef struct {
  mbin_vertex_t entries[VERTEX_STRIDE];
  bool occupied;
} VertexHashKey;

typedef struct {
  VertexHashKey *entries;
  size_t capacity;
  size_t length;
} VertexHashTable;

int vhash_insert(VertexHashTable *, vhash_value_t);
int vhash_create(VertexHashTable *, size_t);
VertexHashKey* vhash_search(VertexHashTable *, vhash_value_t);
#endif
