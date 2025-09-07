#include "list.h"

void vertex_list_create(VertexList *list, size_t capacity) {
  list->count = capacity;
  for(VertexAttributeType i = 0; i < VERTEX_ATTRIBUTE_COUNT; i++)
     list->attributes[i] = (vattr_t *)calloc(3 * list->count, sizeof(vattr_t));
}
