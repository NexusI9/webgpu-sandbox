#ifndef _VINDEX_H_
#define _VINDEX_H_

#include "mbin.h"
#include <stdlib.h>

typedef struct {
  mbin_index_t position;
  mbin_index_t uv;
  mbin_index_t normal;
} IndexAttribute;

typedef struct {
  size_t capacity;
  size_t length;
  IndexAttribute *entries;
} IndexAttributeList;


int index_triangulate();

#endif
