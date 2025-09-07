#ifndef _VATTR_H_
#define _VATTR_H_

#include "mbin.h"
#include <stdio.h>
#include <stdlib.h>

#define VERTEX_LIST_CAPACITY 64
#define VERTEX_COLOR {0.0f, 0.0f, 0.0f}
#define VERTEX_LINE_PREFIX_POSITION "v "
#define VERTEX_LINE_PREFIX_NORMAL "vn "
#define VERTEX_LINE_PREFIX_UV "vt "
#define VERTEX_LINE_PREFIX_UNDEFINED 0
#define VERTEX_SEPARATOR " "

#define VERTEX_STRIDE 15
#define VERTEX_ATTRIBUTE_COUNT 5

typedef enum {
  VertexAttributeListStatus_Success,
  VertexAttributeListStatus_AllocFail,
  VertexAttributeListStatus_UndefError,
} VertexAttributeListStatus;

typedef enum {
  VertexAttributeType_Position,
  VertexAttributeType_Normal,
  VertexAttributeType_Tangent,
  VertexAttributeType_Color,
  VertexAttributeType_Uv,
} VertexAttributeType;

typedef enum {
  VertexAttributeDimension_Position = 3,
  VertexAttributeDimension_Normal = 3,
  VertexAttributeDimension_Tangent = 4,
  VertexAttributeDimension_Color = 3,
  VertexAttributeDimension_Uv = 2,
} VertexAttributeDimension;

typedef struct {
  size_t capacity;
  size_t length;
  vec_dimension_t dimension;
  mbin_vertex_t *entries;
  char *prefix;
  const char *label;
} VertexAttributeList;

typedef struct {
  VertexAttributeList *list;
} VertexAttributeCallbackDescriptor;

void mbin_vertex_attribute_print(VertexAttributeList *);
VertexAttributeListStatus
mbin_vertex_attribute_list_insert(VertexAttributeList *, mbin_vertex_t *,
                                  size_t);
void mbin_vertex_attribute_from_line(const char *, void *);

void mbin_vertex_attribute_free(VertexAttributeList *);
VertexAttributeListStatus mbin_vertex_attribute_copy(VertexAttributeList *,
                                                     VertexAttributeList *);

void mbin_vertex_attribute_set_line_uv(VertexAttributeList *);

#endif
