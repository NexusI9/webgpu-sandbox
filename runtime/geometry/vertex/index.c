#include "index.h"
#include "string.h"

int vertex_index_copy(VertexIndex *src, VertexIndex *dest) {

  if (dest->entries)
    vertex_index_destroy(dest);

  dest->capacity = src->capacity;
  dest->buffer = src->buffer;
  dest->length = src->length;

  size_t length = dest->length * sizeof(vindex_t);
  dest->entries = malloc(length);
  if (dest->entries == NULL) {
    perror("Couldn't allocate memory for vertex index\n");
    dest->buffer = NULL;
    dest->capacity = 0;
    dest->length = 0;
    return VERTEX_ALLOC_FAIL;
  }

  memcpy(dest->entries, src->entries, length);

  return VERTEX_SUCCESS;
}

void vertex_index_destroy(VertexIndex *vi) {
  // free(vi->entries);
  vi->entries = NULL;
  vi->length = 0;
  vi->capacity = 0;
}

void vertex_index_print(VertexIndex *vi) {
  for (size_t i = 0; i < vi->length; i++)
    printf("%u ", vi->entries[i]);
  printf("\n");
}
