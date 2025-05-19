#ifndef _VINDEX_H_
#define _VINDEX_H_

#include "mbin.h"
#include "vattr.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define INDEX_ATTRIBUTE_LINE_PREFIX "f "
#define INDEX_SEPARATOR "/"
#define INDEX_GROUP_SEPARATOR " "

#define INDEX_DEFAULT_CAPACITY 64

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
} VertexIndexCallbackDescriptor;

void index_attribute_cache(FILE *, IndexAttributeList *);
int index_attribute_triangulate(IndexAttributeList *);
void index_attribute_position_list(IndexAttributeGroup *, mbin_index_t *,
                                   size_t *, size_t *);

int index_attribute_compose_from_vertex(IndexAttributeList *,
                                        VertexAttributeList **,
                                        mbin_vertex_t *);

void index_attribute_print(const IndexAttributeList *);

#endif
