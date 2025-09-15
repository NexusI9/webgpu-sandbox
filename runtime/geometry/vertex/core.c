#include "core.h"

void vertex_create(Vertex *vertex) {
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, vertex->position);
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, vertex->normal);
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, vertex->tangent);
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, vertex->color);
  glm_vec2_copy((vec2){0.0f, 0.0f}, vertex->uv);
}

/**
   Output the vertex structure from a data array.
   Useful when going through a mesh vertex and analyse
   its vertex array.
 */
Vertex vertex_from_array(float *data) {

  return (Vertex){
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
      .tangent =
          {
              *(data + 6),
              *(data + 7),
              *(data + 8),
          },
      .color =
          {
              *(data + 9),
              *(data + 10),
              *(data + 11),
          },
      .uv =
          {
              *(data + 12),
              *(data + 13),
          },
  };
}

/**
   Transform vertex into an array
 */
void vertex_to_array(Vertex *vertex, float *array) {

  // copy position
  array[0] = vertex->position[0];
  array[1] = vertex->position[1];
  array[2] = vertex->position[2];

  // copy normal
  array[3] = vertex->normal[0];
  array[4] = vertex->normal[1];
  array[5] = vertex->normal[2];

  // copy tangent
  array[6] = vertex->tangent[0];
  array[7] = vertex->tangent[1];
  array[8] = vertex->tangent[2];

  // copy normal
  array[9] = vertex->color[0];
  array[10] = vertex->color[1];
  array[11] = vertex->color[2];

  // copy uv
  array[12] = vertex->uv[0];
  array[13] = vertex->uv[1];
}

/**
   Copy a vertex array to another array
 */
void vertex_copy(float *src, float *dest) {
  for (int i = 0; i < VERTEX_STRIDE; i++)
    dest[i] = src[i];
}
