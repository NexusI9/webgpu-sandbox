#ifndef _LINE_H_
#define _LINE_H_

#include "vertex.h"
#include <cglm/cglm.h>

#define LINE_SUCCESS 0
#define LINE_ALLOC_FAIL 1
#define LINE_ERROR 2

typedef struct {
  vec3 attribute;
  vindex_t index;
} LinePoint;

typedef struct {
  LinePoint p1;
  LinePoint p2;
} Line;

typedef struct {
  vindex_t *entries;
  size_t capacity;
  size_t length;
} LineListIndex;

typedef struct {
  vattr_t *entries;
  size_t capacity;
  size_t length;
} LineListAttributes;

int line_index_create(LineListIndex *, size_t);
int line_attribute_create(LineListAttributes *, size_t);
void line_create(const Line *, LineListIndex *, LineListAttributes *);

#endif
