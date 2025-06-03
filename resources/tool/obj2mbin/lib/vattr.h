#ifndef _VATTR_H_
#define _VATTR_H_

#include "mbin.h"
#include <stdio.h>
#include <stdlib.h>

#define VERTEX_LIST_CAPACITY 64
#define VERTEX_COLOR {0.0f, 0.0f, 0.0f}
#define VERTEX_POSITION_LINE_PREFIX "v "
#define VERTEX_NORMAL_LINE_PREFIX "vn "
#define VERTEX_UV_LINE_PREFIX "vt "
#define VERTEX_SEPARATOR " "
#define VERTEX_STRIDE 11

#define VERTEX_ATTRIBUTE_LIST_SUCCESS 0
#define VERTEX_ATTRIBUTE_LIST_ALLOC_FAIL 1
#define VERTEX_ATTRIBUTE_LIST_ERROR 2

typedef enum {
  VertexAttributeListIndex_Position = 0,
  VertexAttributeListIndex_Normal = 1,
  VertexAttributeListIndex_Uv = 2,
} VertexAttributeListIndex;

typedef struct {
  size_t capacity;
  size_t length;
  vec_dimension_t dimension;
  mbin_vertex_t *entries;
  char *prefix;
} VertexAttributeList;

typedef struct {
  VertexAttributeList *list;
} VertexAttributeCallbackDescriptor;

void vertex_attribute_print(VertexAttributeList *);
int vertex_attribute_list_insert(VertexAttributeList *, mbin_vertex_t *,
                                 size_t);
void vertex_attribute_from_line(const char *, void *);
void vertex_attribute_cache(FILE *, VertexAttributeList **);

void vertex_attribute_free(VertexAttributeList *);
int vertex_attribute_copy(VertexAttributeList *, VertexAttributeList *);

#endif
