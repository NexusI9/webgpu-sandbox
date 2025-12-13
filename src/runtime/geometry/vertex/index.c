#include "index.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "core.h"
#include "string.h"
#include "backend/logger.h"

VertexStatus vertex_index_copy(VertexIndex *src, VertexIndex *dest) {

  if (dest->entries)
    vertex_index_destroy(dest);

  dest->capacity = src->capacity;
  dest->buffer = src->buffer;
  dest->count = src->count;

  size_t count = dest->count * sizeof(vindex_t);
  dest->entries = malloc(count);
  if (dest->entries == NULL) {
    logger_add(LoggerFlag_Error, "Couldn't allocate memory for vertex index.");
    dest->buffer = NULL;
    dest->capacity = 0;
    dest->count = 0;
    return VertexStatus_AllocFail;
  }

  memcpy(dest->entries, src->entries, count);

  return VertexStatus_Success;
}

void vertex_index_destroy(VertexIndex *vi) {
  free(vi->entries);
  vi->entries = NULL;
  vi->count = 0;
  vi->capacity = 0;
}

void vertex_index_print(VertexIndex *vi) {
  for (size_t i = 0; i < vi->count; i++)
    printf("%u ", vi->entries[i]);
  printf("\n");
}

VertexStatus vertex_index_create(VertexIndex *vi, size_t capacity,
                                 WGPUBuffer buffer) {

  vi->count = 0;
  vi->capacity = capacity;
  vi->entries = calloc(vi->capacity, sizeof(vindex_t));
  vi->buffer = buffer;

  if (vi->entries == NULL) {
    logger_add(LoggerFlag_Error, "Could't create vertex index.");
    vi->capacity = 0;
    vi->buffer = NULL;
    return VertexStatus_AllocFail;
  }

  return VertexStatus_Success;
}

VertexStatus vertex_index_insert(VertexIndex *vi, vindex_t *index_list,
                                 size_t count) {

  // check capacity
  if (vi->count + count >= vi->capacity) {
    size_t new_capacity = vi->count + (2 * vi->capacity);
    vindex_t *temp =
        (vindex_t *)realloc(vi->entries, new_capacity * sizeof(vindex_t));

    if (vi->entries != NULL) {
      vi->capacity = new_capacity;
      vi->entries = temp;
    } else {
      logger_add(LoggerFlag_Error, "Could't reallocate vertex index.");
      return VertexStatus_AllocFail;
    }
  }

  // insert new values
  memcpy(&vi->entries[count], index_list, count * sizeof(vindex_t));

  // incr count
  vi->count += count;

  return VertexStatus_Success;
}
