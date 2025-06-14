#include "list.h"

void vertex_list_create(VertexList *list, size_t count) {

  list->count = count;

  // init raw vertex list attributes
  list->position = (float *)calloc(3 * list->count, sizeof(float)); // vec3
  list->normal = (float *)calloc(3 * list->count, sizeof(float));   // vec3
  list->color = (float *)calloc(3 * list->count, sizeof(float));    // vec3
  list->uv = (float *)calloc(3 * list->count, sizeof(float));       // vec3
}
