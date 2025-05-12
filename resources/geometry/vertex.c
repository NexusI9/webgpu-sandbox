#include "vertex.h"
#include "../utils/vector.h"

void vertex_create(vertex *vertex) {

  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, vertex->position);
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, vertex->normal);
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, vertex->color);
  glm_vec2_copy((vec2){0.0f, 0.0f}, vertex->uv);
}

void vertex_list_create(VertexList *list, size_t count) {

  list->count = count;

  // init raw vertex list attributes
  list->position = (float *)calloc(3 * list->count, sizeof(float)); // vec3
  list->normal = (float *)calloc(3 * list->count, sizeof(float));   // vec3
  list->color = (float *)calloc(3 * list->count, sizeof(float));    // vec3
  list->uv = (float *)calloc(3 * list->count, sizeof(float));       // vec3
}

/**
   Output the vertex structure from a data array.
   Useful when going through a mesh vertex and analyse
   its vertex array.
 */
vertex vertex_from_array(float *data) {

  return (vertex){
      .position =
          {
              *data,
              *(data + 1),
              *(data + 2),
          },
      .normal =
          {
              *(data + 3),
              *(data + 4),
              *(data + 5),
          },
      .color =
          {
              *(data + 6),
              *(data + 7),
              *(data + 8),
          },
      .uv =
          {
              *(data + 9),
              *(data + 10),
          },
  };
}

/**
   Find a vertex with the same given attributes in a vertex attribute array
   Output null if no equivalent found or a list of matching vertex
 */
void vertex_find_equal_attr(vertex *source,
                            VertexAttribute *vertex_attribute,
                            VertexAttributeName attribute,
                            VertexAttribute *destination) {

  for (size_t i = 0; i < vertex_attribute->length; i += VERTEX_STRIDE) {

    if (destination->length == destination->capacity)
      return;

    vertex compare = vertex_from_array(&vertex_attribute->entries[i]);
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

/**
   Transform vertex into an array
 */
void vertex_to_array(vertex *vertex, float *array) {

  // copy position
  array[0] = vertex->position[0];
  array[1] = vertex->position[1];
  array[2] = vertex->position[2];

  // copy normal
  array[3] = vertex->normal[0];
  array[4] = vertex->normal[1];
  array[5] = vertex->normal[2];

  // copy normal
  array[6] = vertex->color[0];
  array[7] = vertex->color[1];
  array[8] = vertex->color[2];

  // copy uv
  array[9] = vertex->uv[0];
  array[10] = vertex->uv[1];
}

/**
   Copy a vertex array to another array
 */
void vertex_copy(float *src, float *dest) {
  for (int i = 0; i < VERTEX_STRIDE; i++)
    dest[i] = src[i];
}
