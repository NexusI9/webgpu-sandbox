#ifndef _VECTOR_UTILS_VEC3_LIST_H_
#define _VECTOR_UTILS_VEC3_LIST_H_

#include <cglm/cglm.h>
#include <stddef.h>
#include <cglm/types.h>

#include "utils/dyli.h"

typedef struct {
  vec3 *entries;
  size_t count;
  size_t capacity;
} Vec3List;

DynamicListStatus vec3_list_create(Vec3List *, size_t);
DynamicListStatus vec3_list_insert(Vec3List *, vec3);
DynamicListStatus vec3_list_empty(Vec3List *);
void vec3_list_destroy(Vec3List *);

#endif
