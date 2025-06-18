#ifndef _VERTEX_GROUP_H_
#define _VERTEX_GROUP_H_
#include "core.h"
#include "index.h"
#include <stddef.h>
#include <stdint.h>

#define VERTEX_GROUP_CAPACITY_DEFAULT 53
#define VERTEX_GROUP_SUCCESS 0
#define VERTEX_GROUP_ALLOC_FAIL 1
#define VERTEX_GROUP_ERROR 2
#define VERTEX_GROUP_SET_UNFOUND 3

typedef const char *vgroup_key;
typedef uint32_t vgroup_hash;

typedef struct {
  char *name;
  vindex_t *entries;
  size_t length;
  size_t capacity;
} VertexGroup;

typedef struct {
  VertexGroup *entries;
  size_t length;
  size_t capacity;
} VertexGroupSet;

/*Vertex Group*/
int vertex_group_create(VertexGroup *, size_t, const char *);
VertexGroup *vertex_group_insert(VertexGroup *, vindex_t *, size_t);
void vertex_group_free(VertexGroup *);

/*Vertex Group Set*/
int vertex_group_set_create(VertexGroupSet *, size_t);
VertexGroup *vertex_group_set_insert(VertexGroupSet *, VertexGroup *);
VertexGroup *vertex_group_set_find(VertexGroupSet *, vgroup_key);
int vertex_group_set_delete(VertexGroupSet *, vgroup_key);
void vertex_group_set_free(VertexGroupSet *);

#endif
