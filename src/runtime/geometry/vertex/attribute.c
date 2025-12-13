#include "attribute.h"

#include <stdio.h>
#include <stdlib.h>

#include "backend/logger.h"
#include "core.h"
#include "string.h"
#include "utils/vector/core.h"

static inline void vertex_attribute_replace(VertexAttribute *, const float *,
                                            const VertexAttributeOffset,
                                            const size_t);
static inline void vertex_attribute_add(VertexAttribute *, const float *,
                                        const VertexAttributeOffset,
                                        const size_t);

void vertex_attribute_print(VertexAttribute *va) {
  for (size_t i = 0; i < va->count; i++) {
    printf("%f ", va->entries[i]);
    if (i % VERTEX_STRIDE == VERTEX_STRIDE - 1)
      printf("\n");
  }
}

/*
   ▗▄▄▖ ▗▄▄▄▖▗▄▄▖ ▗▖    ▗▄▖  ▗▄▄▖▗▄▄▄▖
   ▐▌ ▐▌▐▌   ▐▌ ▐▌▐▌   ▐▌ ▐▌▐▌   ▐▌
   ▐▛▀▚▖▐▛▀▀▘▐▛▀▘ ▐▌   ▐▛▀▜▌▐▌   ▐▛▀▀▘
   ▐▌ ▐▌▐▙▄▄▖▐▌   ▐▙▄▄▖▐▌ ▐▌▝▚▄▄▖▐▙▄▄▖

 */

/**
   Replace the attributes of a vertex attribute list starting at a certain index
 */
void vertex_attribute_replace(VertexAttribute *va, const float *val,
                              VertexAttributeOffset offset, size_t type_size) {
  for (size_t i = offset; i < va->count; i += VertexAttributeOffset_End)
    memcpy(&va->entries[i], val, type_size);
}

/**
   Replace the color attributes of a vertex attribute list
 */
void vertex_attribute_set_color(VertexAttribute *va, const vertex_color color) {
  vertex_attribute_replace(va, color, VertexAttributeOffset_Color,
                           sizeof(vertex_color));
}

/**
   Replace the color attributes of a vertex attribute list
 */
void vertex_attribute_set_position(VertexAttribute *va,
                                   const vertex_position position) {
  vertex_attribute_replace(va, position, VertexAttributeOffset_Position,
                           sizeof(vertex_position));
}

/**
   Replace the color attributes of a vertex attribute list
 */
void vertex_attribute_set_normal(VertexAttribute *va,
                                 const vertex_normal normal) {
  vertex_attribute_replace(va, normal, VertexAttributeOffset_Normal,
                           sizeof(vertex_normal));
}

/**
   Replace the uv attributes of a vertex attribute list
 */
void vertex_attribute_set_uv(VertexAttribute *va, const vertex_uv uv) {
  vertex_attribute_replace(va, uv, VertexAttributeOffset_Uv, sizeof(vertex_uv));
}

/**
   Replace the color attributes of a vertex attribute list
 */
void vertex_attribute_set_color_at_index(VertexAttribute *va,
                                         const vertex_color color,
                                         const int index) {
  memcpy(&va->entries[index * VERTEX_STRIDE + VertexAttributeOffset_Color],
         color, sizeof(vertex_color));
}

/**
   Replace the color attributes of a vertex attribute list
 */
void vertex_attribute_set_position_at_index(VertexAttribute *va,
                                            const vertex_position position,
                                            const int index) {
  memcpy(&va->entries[index * VERTEX_STRIDE + VertexAttributeOffset_Position],
         position, sizeof(vertex_position));
}

/**
   Replace the color attributes of a vertex attribute list
 */
void vertex_attribute_set_normal_at_index(VertexAttribute *va,
                                          const vertex_normal normal,
                                          const int index) {
  memcpy(&va->entries[index * VERTEX_STRIDE + VertexAttributeOffset_Normal],
         normal, sizeof(vertex_normal));
}

/**
   Replace the uv attributes of a vertex attribute list
 */
void vertex_attribute_set_uv_at_index(VertexAttribute *va, const vertex_uv uv,
                                      const int index) {
  memcpy(&va->entries[index * VERTEX_STRIDE + VertexAttributeOffset_Uv], uv,
         sizeof(vertex_uv));
}

/*
   ▗▄▖ ▗▄▄▄ ▗▄▄▄
  ▐▌ ▐▌▐▌  █▐▌  █
  ▐▛▀▜▌▐▌  █▐▌  █
  ▐▌ ▐▌▐▙▄▄▀▐▙▄▄▀

 */

/**
   Add up the attributes of a vertex attribute list starting at a certain index
 */
void vertex_attribute_add(VertexAttribute *va, const float *val,
                          VertexAttributeOffset offset, size_t type_size) {

  size_t count = type_size / sizeof(vattr_t);
  for (size_t i = offset; i < va->count; i += VertexAttributeOffset_End) {
    vattr_t *dst = (vattr_t *)&va->entries[i];
    for (size_t j = 0; j < count; j++)
      dst[j] += val[j];
  }
}

/**
   Add up the color attributes of a vertex attribute list
 */
void vertex_attribute_set_color_add(VertexAttribute *va,
                                    const vertex_color color) {
  vertex_attribute_add(va, color, VertexAttributeOffset_Color,
                       sizeof(vertex_color));
}

/**
   Add up the color attributes of a vertex attribute list
 */
void vertex_attribute_set_position_add(VertexAttribute *va,
                                       const vertex_position position) {
  vertex_attribute_add(va, position, VertexAttributeOffset_Position,
                       sizeof(vertex_position));
}

/**
   Add up the color attributes of a vertex attribute list
 */
void vertex_attribute_set_normal_add(VertexAttribute *va,
                                     const vertex_normal normal) {
  vertex_attribute_add(va, normal, VertexAttributeOffset_Normal,
                       sizeof(vertex_normal));
}

/**
   Add up the uv attributes of a vertex attribute list
 */
void vertex_attribute_set_uv_add(VertexAttribute *va, const vertex_uv uv) {
  vertex_attribute_add(va, uv, VertexAttributeOffset_Uv, sizeof(vertex_uv));
}

/**

   ▗▖ ▗▖▗▄▄▄▖▗▄▄▄▖▗▖    ▗▄▄▖
   ▐▌ ▐▌  █    █  ▐▌   ▐▌
   ▐▌ ▐▌  █    █  ▐▌    ▝▀▚▖
   ▝▚▄▞▘  █  ▗▄█▄▖▐▙▄▄▖▗▄▄▞▘

 */

VertexStatus vertex_attribute_copy(VertexAttribute *src,
                                   VertexAttribute *dest) {

  if (dest->entries)
    vertex_attribute_destroy(dest);

  dest->capacity = src->capacity;
  dest->buffer = src->buffer;
  dest->count = src->count;

  size_t count = dest->count * sizeof(vattr_t);
  dest->entries = malloc(count);
  if (dest->entries == NULL) {
    logger_add(LoggerFlag_Error,
               "Couldn't allocate memory for vertex attribute.");
    dest->buffer = NULL;
    dest->capacity = 0;
    dest->count = 0;
    return VertexStatus_AllocFail;
  }

  memcpy(dest->entries, src->entries, count);

  return VertexStatus_Success;
}

void vertex_attribute_destroy(VertexAttribute *va) {
  free(va->entries);
  va->entries = NULL;
  va->count = 0;
  va->capacity = 0;
}

/**
   Find a vertex with the same given attributes in a vertex attribute array
   Output null if no equivalent found or a list of matching vertex

   (Unused)
 */
void vertex_attribute_find_equal_attr(Vertex *source,
                                      VertexAttribute *vertex_attribute,
                                      VertexAttributeType attribute,
                                      VertexAttribute *destination) {

  for (size_t i = 0; i < vertex_attribute->count; i += VERTEX_STRIDE) {

    if (destination->count == destination->capacity)
      return;

    Vertex compare = vertex_from_array(&vertex_attribute->entries[i]);
    float *v_src = &vertex_attribute->entries[i];
    float *v_dest = &destination->entries[destination->count];

    // position match
    if (attribute == VertexAttributeType_Position &&
        vec3_equal(source->position, compare.position)) {
      vertex_copy(v_src, v_dest);
      destination->count += VERTEX_STRIDE;
      continue;
    }

    // normal match
    if (attribute == VertexAttributeType_Normal &&
        vec3_equal(source->normal, compare.normal)) {
      vertex_copy(v_src, v_dest);
      destination->count += VERTEX_STRIDE;
      continue;
    }

    // tangent match
    if (attribute == VertexAttributeType_Tangent &&
        vec3_equal(source->tangent, compare.tangent)) {
      vertex_copy(v_src, v_dest);
      destination->count += VERTEX_STRIDE;
      continue;
    }

    // color match
    if (attribute == VertexAttributeType_Color &&
        vec3_equal(source->color, compare.color)) {
      vertex_copy(v_src, v_dest);
      destination->count += VERTEX_STRIDE;
      continue;
    }

    // uv match
    if (attribute == VertexAttributeType_Uv &&
        vec2_equal(source->uv, compare.uv)) {
      vertex_copy(v_src, v_dest);
      destination->count += VERTEX_STRIDE;
      continue;
    }
  }
}
