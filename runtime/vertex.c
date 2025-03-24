#include "vertex.h"

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
