#ifndef _V_HASH_H_
#define _V_HASH_H_

#include "./mbin.h"
#include "vindex.h"
#include <stdbool.h>

/*
  Need to use an non odd number for base capacity (non power of 2 value)
  When using a power of two (2^N), the moduulo operation become mathematically
  equal to a Bitwise operation:

  ∆ % 2^N  == ∆ & (2^N) - 1  ||  ∆ % 128 == ∆ & 127

  In bitwise operation, only the lowest N bits of the hash value is used to
  define the final bucket. Which leads to imprecision since the higher bits are
  discarded.

  The number of used bits is N (for 128 = 2^7, the number of used bits would be 7)
 */
#define VHASH_BASE_CAPACITY 127
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
