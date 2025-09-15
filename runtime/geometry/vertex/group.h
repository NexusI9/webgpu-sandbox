#ifndef _VERTEX_GROUP_H_
#define _VERTEX_GROUP_H_
#include <stddef.h>
#include <stdint.h>

#include "core.h"
#include "index.h"

#define VERTEX_GROUP_CAPACITY_DEFAULT 53

typedef enum {
  VertexGroupStatus_Success,
  VertexGroupStatus_AllocFail,
  VertexGroupStatus_UnderError,
  VertexGroupStatus_SetUnfound,
} VertexGroupStatus;


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
VertexGroupStatus vertex_group_create(VertexGroup *, size_t, const char *);
VertexGroup *vertex_group_insert(VertexGroup *, vindex_t *, size_t);
void vertex_group_free(VertexGroup *);

/*Vertex Group Set*/
VertexGroupStatus vertex_group_set_create(VertexGroupSet *, size_t);
VertexGroup *vertex_group_set_insert(VertexGroupSet *, VertexGroup *);
VertexGroup *vertex_group_set_find(VertexGroupSet *, vgroup_key);
VertexGroupStatus vertex_group_set_delete(VertexGroupSet *, vgroup_key);
void vertex_group_set_free(VertexGroupSet *);

#endif
