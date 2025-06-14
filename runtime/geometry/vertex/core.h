#ifndef _VERTEX_CORE_H_
#define _VERTEX_CORE_H_

#include <cglm/cglm.h>
#include <stddef.h>
#include <webgpu/webgpu.h>

#define VERTEX_STRIDE 11

#define VERTEX_SUCCESS 0
#define VERTEX_ALLOC_FAIL 1

typedef vec2 vertex_uv;
typedef vec3 vertex_color;
typedef vec3 vertex_position;
typedef vec3 vertex_normal;

typedef enum {
  VertexOffset_Position = 0,
  VertexOffset_Normal = 3,
  VertexOffset_Color = 6,
  VertexOffset_Uv = 9,
  VertexOffset_End = VERTEX_STRIDE,
} VertexOffset;


/*
  Cannonical structure of a vertex
 */
typedef struct {
  vec3 position;
  vec3 normal;
  vec3 color;
  vec2 uv;
} Vertex;

typedef enum {
  VertexAttributeName_Undefined = 0,
  VertexAttributeName_Position = 1 << 0,
  VertexAttributeName_Normal = 1 << 1,
  VertexAttributeName_Color = 1 << 2,
  VertexAttributeName_Uv = 1 << 3,
  VertexAttributeName_All = 1 << 4,
} VertexAttributeName;

void vertex_create(Vertex *);
Vertex vertex_from_array(float *);

void vertex_to_array(Vertex *, float *);
void vertex_copy(float *, float *);



#endif
