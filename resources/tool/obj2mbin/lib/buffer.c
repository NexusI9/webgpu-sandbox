#include "buffer.h"
#include "mbin.h"
#include "string.h"
#include "vattr.h"
#include <stddef.h>
#include <stdio.h>

static void vertex_buffer_print(VertexBuffer *vb) {

  for (size_t v = 0; v < vb->length; v++) {
    printf("%f ", vb->entries[v]);
    if (v % VERTEX_STRIDE == VERTEX_STRIDE - 1)
      printf("\n");
  }

  printf("\n");
}

static void index_buffer_print(IndexBuffer *ib) {

  for (size_t i = 0; i < ib->length; i++)
    printf("%d ", ib->entries[i]);

  printf("\n");
}

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

/**
   Takes the Vertex and Index buffers data and respectively merge them into
   one array. Need to combine them cause pointers array are pointless in
   binary, so need to put a data[] at the end of the struct which lead to
   combine all data into one block and them manually decode them depending on
   the vertex index length/typesize. Actually similare to GLTF accessro and
   offset principle.

    00001010010101010111110101010010101010101010010101010101010101010100101001010100101
   '--------------------.------------------''-----------------------------------------'
              nVertex * sizeof(vertex)      +       nIndex * sizeof(Index)

*/
int buffer_merge_data(VertexBuffer *vb, IndexBuffer *ib, mbin_data_t *dest) {

  size_t vert_size = sizeof(mbin_vertex_t) * vb->length;
  size_t index_size = sizeof(mbin_index_t) * ib->length;

#ifdef VERBOSE
  printf("\nVertex:\n");
  vertex_buffer_print(vb);
  printf("\nIndex:\n");
  index_buffer_print(ib);
#endif

  memcpy(dest, vb->entries, vert_size);
  /*NOTE: for pointer arithmetic:
    dest is uint32_t*

    which meanns that:
    dest + N => dest + N * sizeof(uint32_t)

    When navigating through pointers we don't need to do it in bytes (sizeof(T)
    * length) We can omit the sizeof(T) cause the step will be defined based on
    dest type
   */
  memcpy(dest + vb->length, ib->entries, index_size);

  return MBIN_BUFFER_SUCCESS;
}
