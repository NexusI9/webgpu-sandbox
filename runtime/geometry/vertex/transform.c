#include "transform.h"
#include "../utils/system.h"
#include "attribute.h"
#include "core.h"
#include "index.h"
#include <stdint.h>
#include <string.h>

static void vertex_transform_origin(const VertexIndex *,
                                    const VertexAttribute *, vec3 *);

static float *vertex_transform_attribute(const vindex_t,
                                         const VertexAttribute *, uint8_t);

/**
   Adjust the vertex attribute properties depending on given indexes.
*/
void vertex_transform_scale(const VertexIndex *index,
                            VertexAttribute *attribute, vec3 *scale) {

  // get origin
  vec3 origin;
  vertex_transform_origin(index, attribute, &origin);

  for (size_t i = 0; i < index->length; i++) {

    vindex_t id = index->entries[i];

    // get position pointer according to index
    float *position =
        vertex_transform_attribute(id, attribute, VertexOffset_Position);

    /*DELETEME:*/
    printf("[%u] before: ", id);
    print_vec3(position);

    // get direction
    vec3 direction;
    glm_vec3_sub(position, origin, direction);

    // start from Origin, move along PO direction * scale
    // Scaled P = O + AO * scale

    glm_vec3_muladd(direction, *scale, origin);
    glm_vec3_copy(origin, position);

    /*DELETEME:*/
    printf("[%u] after: ", id);
    print_vec3(position);
  }
}

/**
   Adjust the vertex attribute properties depending on given indexes.
 */
void vertex_transform_translate(const VertexIndex *index,
                                VertexAttribute *attribute, vec3 *translation) {

  // get origin
  vec3 origin;
  vertex_transform_origin(index, attribute, &origin);
}

/**
   Adjust the vertex attribute properties depending on given indexes.
 */
void vertex_transform_rotate(const VertexIndex *index,
                             VertexAttribute *attribute, vec3 *rotation) {}

/**
   Get the attribute at the given index
 */
float *vertex_transform_attribute(const vindex_t index,
                                  const VertexAttribute *attribute,
                                  uint8_t offset) {
  return &attribute->entries[index * VERTEX_STRIDE + offset];
}

/**
   Get the origin of the given vertex index
 */
static void vertex_transform_origin(const VertexIndex *index,
                                    const VertexAttribute *attribute,
                                    vec3 *dest) {
  // init
  glm_vec3_zero(*dest);

  for (size_t i = 0; i < index->length; i++) {

    vindex_t id = index->entries[i];

    // get position from index
    float *position =
        vertex_transform_attribute(id, attribute, VertexOffset_Position);

    // add
    glm_vec3_add(*dest, position, *dest);
  }

  // divide
  glm_vec3_div(*dest, (vec3){index->length, index->length, index->length},
               *dest);
}

/**
   Get all the index that share the same position
 */
void vertex_transform_alike_by_position(const VertexIndex *index_list,
                                        const VertexAttribute *attribute_list,
                                        vertex_position *position,
                                        VertexIndex *dest) {

  // init dest list
  if (vertex_index_create(dest, 32, NULL) != VERTEX_SUCCESS) {
    printf("Vertex index creation fail");
    return;
  }

  // traverse compare
  for (size_t i = 0; i < index_list->length; i++) {

    vindex_t index = index_list->entries[i];
    vattr_t *vertex = &attribute_list->entries[index * VERTEX_STRIDE];
    if (memcmp(position, vertex, sizeof(vertex_position)) == 0) {
      // add to dest
      vertex_index_insert(dest, &index, 1);
    }
  }
}

/**
   Get all the index that share the same position with the given index
 */
void vertex_transform_alike_by_index(const VertexIndex *index_list,
                                     const VertexAttribute *attribute_list,
                                     vindex_t index, VertexIndex *dest) {

  // retrieve position from index
  vattr_t *v = &attribute_list->entries[index * VERTEX_STRIDE];
  vertex_position position = {v[0], v[1], v[2]};

  // get alike from above position
  vertex_transform_alike_by_position(index_list, attribute_list, &position,
                                     dest);
}
