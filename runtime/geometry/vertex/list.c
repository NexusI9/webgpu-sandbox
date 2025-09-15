#include "list.h"

#include <stdlib.h>

#include "core.h"

void vertex_list_create(VertexList *list, size_t capacity) {

  static const VertexAttributeDimension
      type_capacities[VERTEX_ATTRIBUTE_COUNT] = {
          [VertexAttributeType_Position] = VertexAttributeDimension_Position,
          [VertexAttributeType_Normal] = VertexAttributeDimension_Normal,
          [VertexAttributeType_Tangent] = VertexAttributeDimension_Tangent,
          [VertexAttributeType_Color] = VertexAttributeDimension_Color,
          [VertexAttributeType_Uv] = VertexAttributeDimension_Uv,
      };

  list->count = capacity;
  for (VertexAttributeType i = 0; i < VERTEX_ATTRIBUTE_COUNT; i++)
    list->attributes[i] =
        (vattr_t *)calloc(type_capacities[i] * list->count, sizeof(vattr_t));
}
