#ifndef _VATTR_H_
#define _VATTR_H_

#include "mbin.h"
#include <stdlib.h>

typedef struct {
  size_t capacity;
  size_t length;
  vec_dimension_t dimension;
  mbin_vertex_t *entries;
  const char *prefix;
} VertexAttributeList;

typedef struct {
  VertexAttributeList *list;
} VertexAttributeCallbackDescriptor;


void vertex_attribute_list_print(VertexAttributeList *);
int vertex_attribute_list_insert(mbin_vertex_t, VertexAttributeList *);
void vertex_attribute_from_line(const char *, void *);

#endif
