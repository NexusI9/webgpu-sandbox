#include "transform.h"
#include "../utils/system.h"
#include "attribute.h"
#include "core.h"
#include "index.h"
#include <stdint.h>
#include <string.h>

static void vertex_transform_origin(const VertexIndexSelection *,
                                    const VertexAttribute *, vec3 *);

static float *vertex_transform_attribute(const vindex_t,
                                         const VertexAttribute *, uint8_t);

/**
   Adjust the vertex attribute properties depending on given indexes.
*/
void vertex_transform_scale(const VertexIndexSelection *index,
                            VertexAttribute *attribute, vec3 *scale) {

  // get origin
  vec3 origin;
  vertex_transform_origin(index, attribute, &origin);

  for (size_t i = 0; i < index->length; i++) {

    vindex_t id = index->entries[i];

    // get position pointer according to index
    float *position =
        vertex_transform_attribute(id, attribute, VertexOffset_Position);

    // get direction
    vec3 direction;
    glm_vec3_sub(position, origin, direction);

    // start from Origin, move along PO direction * scale
    // Scaled P = O + AO * scale

    glm_vec3_mul(direction, *scale, direction);
    glm_vec3_add(origin, direction, position);

  }
}

/**
   Adjust the vertex attribute properties depending on given indexes.
 */
void vertex_transform_translate(const VertexIndexSelection *index,
                                VertexAttribute *attribute, vec3 *translation) {

  // get origin
  vec3 origin;
  vertex_transform_origin(index, attribute, &origin);
  
}

/**
   Adjust the vertex attribute properties depending on given indexes.
 */
void vertex_transform_rotate(const VertexIndexSelection *index,
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
static void vertex_transform_origin(const VertexIndexSelection *index,
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

