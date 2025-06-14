#include "attribute.h"
#include "../utils/vector.h"
#include "string.h"

static void vertex_attribute_replace(VertexAttribute *, float *, VertexOffset,
                                     size_t);

void vertex_attribute_print(VertexAttribute *va) {
  for (size_t i = 0; i < va->length; i++) {
    printf("%f ", va->entries[i]);
    if (i % VERTEX_STRIDE == VERTEX_STRIDE - 1)
      printf("\n");
  }
}

/**
   Replace the attributes of a vertex attribute list starting at a certain index
 */
void vertex_attribute_replace(VertexAttribute *va, float *val,
                              VertexOffset offset, size_t type_size) {
  for (size_t i = offset; i < va->length; i += VertexOffset_End)
    memcpy(&va->entries[i], val, type_size);
}

/**
   Replace the color attributes of a vertex attribute list
 */
void vertex_attribute_set_color(VertexAttribute *va, vertex_color *color) {
  vertex_attribute_replace(va, *color, VertexOffset_Color,
                           sizeof(vertex_color));
}

/**
   Replace the color attributes of a vertex attribute list
 */
void vertex_attribute_set_position(VertexAttribute *va,
                                   vertex_position *position) {
  vertex_attribute_replace(va, *position, VertexOffset_Position,
                           sizeof(vertex_position));
}

/**
   Replace the color attributes of a vertex attribute list
 */
void vertex_attribute_set_normal(VertexAttribute *va, vertex_normal *normal) {
  vertex_attribute_replace(va, *normal, VertexOffset_Normal,
                           sizeof(vertex_normal));
}

/**
   Replace the uv attributes of a vertex attribute list
 */
void vertex_attribute_set_uv(VertexAttribute *va, vertex_uv *uv) {
  vertex_attribute_replace(va, *uv, VertexOffset_Uv, sizeof(vertex_uv));
}

int vertex_attribute_copy(VertexAttribute *src, VertexAttribute *dest) {

  if (dest->entries)
    vertex_attribute_destroy(dest);

  dest->capacity = src->capacity;
  dest->buffer = src->buffer;
  dest->length = src->length;

  size_t length = dest->length * sizeof(vattr_t);
  dest->entries = malloc(length);
  if (dest->entries == NULL) {
    perror("Couldn't allocate memory for vertex attribute\n");
    dest->buffer = NULL;
    dest->capacity = 0;
    dest->length = 0;
    return VERTEX_ALLOC_FAIL;
  }

  memcpy(dest->entries, src->entries, length);

  return VERTEX_SUCCESS;
}

void vertex_attribute_destroy(VertexAttribute *va) {
  // free(va->entries);
  va->entries = NULL;
  va->length = 0;
  va->capacity = 0;
}

/**
   Find a vertex with the same given attributes in a vertex attribute array
   Output null if no equivalent found or a list of matching vertex
 */
void vertex_attribute_find_equal_attr(Vertex *source,
                                      VertexAttribute *vertex_attribute,
                                      VertexAttributeName attribute,
                                      VertexAttribute *destination) {

  for (size_t i = 0; i < vertex_attribute->length; i += VERTEX_STRIDE) {

    if (destination->length == destination->capacity)
      return;

    Vertex compare = vertex_from_array(&vertex_attribute->entries[i]);
    float *v_src = &vertex_attribute->entries[i];
    float *v_dest = &destination->entries[destination->length];

    // position match
    if (attribute & VertexAttributeName_Position &&
        vec3_equal(source->position, compare.position)) {
      vertex_copy(v_src, v_dest);
      destination->length += VERTEX_STRIDE;
      continue;
    }

    // normal match
    if (attribute & VertexAttributeName_Normal &&
        vec3_equal(source->normal, compare.normal)) {
      vertex_copy(v_src, v_dest);
      destination->length += VERTEX_STRIDE;
      continue;
    }

    // color match
    if (attribute & VertexAttributeName_Color &&
        vec3_equal(source->color, compare.color)) {
      vertex_copy(v_src, v_dest);
      destination->length += VERTEX_STRIDE;
      continue;
    }

    // uv match
    if (attribute & VertexAttributeName_Uv &&
        vec2_equal(source->uv, compare.uv)) {
      vertex_copy(v_src, v_dest);
      destination->length += VERTEX_STRIDE;
      continue;
    }
  }
}
