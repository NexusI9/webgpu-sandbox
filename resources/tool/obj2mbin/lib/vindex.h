#ifndef _VINDEX_H_
#define _VINDEX_H_

#include "buffer.h"
#include "mbin.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define VINDEX_ATTRIBUTE_LINE_PREFIX "f "
#define VINDEX_SEPARATOR "/"
#define VINDEX_GROUP_SEPARATOR " "

#define VINDEX_DEFAULT_CAPACITY 64
#define VINDEX_SUCCESS 0
#define VINDEX_ALLOC_FAILURE 1

/* Structure list
  List > Group > Attribute
 */

typedef struct {
  mbin_index_t position;
  mbin_index_t uv;
  mbin_index_t normal;
} IndexAttribute;

typedef struct {
  size_t capacity;
  size_t length;
  IndexAttribute *entries;
} IndexAttributeGroup;

typedef struct {
  size_t capacity;
  size_t length;
  IndexAttributeGroup *entries;
} IndexAttributeList;

typedef struct {
  IndexAttributeList *list;
  const char *pattern;
} VertexIndexCallbackDescriptor;


void index_attribute_cache(FILE *, IndexAttributeList *, const char *,
                           const char *);
int index_attribute_triangulate(IndexAttributeList *);
void index_attribute_position_list(IndexAttributeGroup *, mbin_index_t *,
                                   size_t *, size_t *);

void index_attribute_print(const IndexAttributeList *);
void index_attribute_copy(IndexAttribute *, IndexAttribute *);

void index_attribute_line_set_opposite(IndexAttributeList *);
void index_attribute_line_set_doublon(IndexAttributeList *);

#endif
