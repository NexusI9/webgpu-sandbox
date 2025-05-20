#include "buffer.h"
#include "mbin.h"
#include "string.h"
#include "vattr.h"
#include <stddef.h>
#include <stdio.h>

int vertex_buffer_insert(VertexBuffer *vb, mbin_vertex_t *list, size_t count) {

  if (vb->length + count >= vb->capacity) {

    size_t new_capacity = 2 * vb->capacity;
    void *temp = realloc(vb->entries, new_capacity * sizeof(mbin_vertex_t));

    if (temp) {
      vb->entries = temp;
      vb->capacity = new_capacity;
    } else {
      perror("Couldn't allocate new memory for vertex buffer\n");
      return MBIN_BUFFER_ALLOC_FAILURE;
    }
  }

  memcpy(&vb->entries[vb->length], list, sizeof(mbin_vertex_t) * count);
  vb->length += count;

  return MBIN_BUFFER_SUCCESS;
}

int index_buffer_insert(IndexBuffer *ib, mbin_index_t index) {

  if (ib->length == ib->capacity) {

    size_t new_capacity = 2 * ib->capacity;
    void *temp = realloc(ib->entries, new_capacity * sizeof(mbin_index_t));

    if (temp) {
      ib->entries = temp;
      ib->capacity = new_capacity;
    } else {
      perror("Couldn't allocate new memory for index buffer\n");
      return MBIN_BUFFER_ALLOC_FAILURE;
    }
  }

  ib->entries[ib->length++] = index;

  return MBIN_BUFFER_SUCCESS;
}

void vertex_buffer_free(VertexBuffer *vb) {
  free(vb->entries);
  vb->entries = NULL;
}

void index_buffer_free(IndexBuffer *ib) {
  free(ib->entries);
  ib->entries = NULL;
}
