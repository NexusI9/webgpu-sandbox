#ifndef _V_HASH_H_
#define _V_HASH_H_

#include "./mbin.h"
#include "vindex.h"
#include <stdbool.h>

#define VHASH_BASE_CAPACITY 128
#define VHASH_SUCCESS 0
#define VHASH_ALLOC_FAILURE 1
#define VHASH_EXIST 2

typedef mbin_vertex_t *vhash_value_t;

typedef struct {
  mbin_vertex_t entries[VERTEX_STRIDE];
  mbin_index_t index;
  bool occupied;
} VertexHashKey;

typedef struct {
  VertexHashKey *entries;
  VertexHashKey **occupied_entries;
  size_t capacity;
  size_t length;
} VertexHashTable;

int vhash_insert(VertexHashTable *, vhash_value_t, mbin_index_t *);
int vhash_create(VertexHashTable *, size_t);
VertexHashKey *vhash_search(VertexHashTable *, vhash_value_t);
#endif
