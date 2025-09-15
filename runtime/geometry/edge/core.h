#ifndef _EDGE_CORE_H_
#define _EDGE_CORE_H_

#include <cglm/cglm.h>

typedef struct {
  ivec3 a;
  ivec3 b;
} Edge;

typedef ivec2 EdgeKey;

typedef struct {
  Edge *entries;
  size_t length;
  size_t capacity;
} EdgeList;

typedef struct {
  EdgeKey key;
  bool occupied;
} EdgeBucket;

typedef struct {
  EdgeBucket *entries;
  size_t *occupied;
  size_t length;
  size_t capacity;
} EdgeHashSet;

void edge_hash_set_create(EdgeHashSet *, size_t);
void edge_hash_set_destroy(EdgeHashSet *);
bool edge_hash_set_insert(EdgeHashSet *, EdgeKey);
bool edge_hash_set_find(const EdgeHashSet *, EdgeKey);

bool edge_key_is_null(EdgeKey);

#endif
