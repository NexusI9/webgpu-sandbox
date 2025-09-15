#ifndef _VERTEX_LIST_H_
#define _VERTEX_LIST_H_

#include <stddef.h>

#include "attribute.h"
#include "core.h"
#include "index.h"

/*
  List of mesh vertex attributes and index
 */
typedef struct {

  vattr_t *attributes[VERTEX_ATTRIBUTE_COUNT];
  vindex_t *index;
  size_t count;

} VertexList; // TODO: unsure about the naming..

void vertex_list_create(VertexList *, size_t);

#endif
