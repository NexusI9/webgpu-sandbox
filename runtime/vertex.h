#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "../include/cglm/vec2.h"
#include "../include/cglm/vec3.h"
#include <stddef.h>

#define VERTEX_STRIDE 11
/*
  Cannonical structure of a vertex
 */
typedef struct {

  vec3 position;
  vec3 normal;
  vec3 color;
  vec2 uv;
} vertex;

typedef struct {
  float *data;
  size_t length;
} vertex_attribute;

typedef struct {
  uint16_t *data;
  size_t length;
} vertex_index;

/*
  List of mesh vertex attributes and index
 */
typedef struct {

  float *position;
  float *normal;
  float *color;
  float *uv;

  uint16_t *index;
  size_t count;

} vertex_list; // TODO: unsure about the naming..

void vertex_create(vertex *);
void vertex_list_create(vertex_list *, size_t);

#endif
