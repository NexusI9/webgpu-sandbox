#include "index.h"
#include "string.h"
#include <stddef.h>

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

int vertex_index_create(VertexIndex *vi, size_t capacity, WGPUBuffer buffer) {

  vi->length = 0;
  vi->capacity = capacity;
  vi->entries = calloc(vi->capacity, sizeof(vindex_t));
  vi->buffer = buffer;

  if (vi->entries == NULL) {
    perror("Could't create vertex index\n");
    vi->capacity = 0;
    vi->buffer = NULL;
    return VERTEX_ALLOC_FAIL;
  }

  return VERTEX_SUCCESS;
}

int vertex_index_insert(VertexIndex *vi, vindex_t *index_list, size_t length) {

  // check capacity
  if (vi->length + length >= vi->capacity) {
    size_t new_capacity = vi->length + (2 * vi->capacity);
    vindex_t *temp =
        (vindex_t *)realloc(vi->entries, new_capacity * sizeof(vindex_t));

    if (vi->entries != NULL) {
      vi->capacity = new_capacity;
      vi->entries = temp;
    } else {
      perror("Could't reallocate vertex index\n");
      return VERTEX_ALLOC_FAIL;
    }
  }

  // insert new values
  memcpy(&vi->entries[length], index_list, length * sizeof(vindex_t));

  // incr length
  vi->length += length;

  return VERTEX_SUCCESS;
}
