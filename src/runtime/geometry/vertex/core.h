#ifndef _VERTEX_CORE_H_
#define _VERTEX_CORE_H_

#include <cglm/cglm.h>
#include <stddef.h>
#include <webgpu/webgpu.h>
#include <cglm/types.h>

#define VERTEX_STRIDE 15
#define VERTEX_ATTRIBUTE_COUNT 5

typedef enum {
  VertexStatus_Success,
  VertexStatus_AllocFail,
} VertexStatus;

typedef vec2 vertex_uv;
typedef vec3 vertex_color;
typedef vec3 vertex_position;
typedef vec3 vertex_normal;
typedef vec4 vertex_tangent;

typedef enum{
  VertexAttributeType_Position,
  VertexAttributeType_Normal,
  VertexAttributeType_Tangent,
  VertexAttributeType_Color,
  VertexAttributeType_Uv,
} VertexAttributeType;

typedef enum {
  VertexAttributeOffset_Position = 0,
  VertexAttributeOffset_Normal = 3,
  VertexAttributeOffset_Tangent = 6,
  VertexAttributeOffset_Color = 10,
  VertexAttributeOffset_Uv = 13,
  VertexAttributeOffset_End = VERTEX_STRIDE,
} VertexAttributeOffset;

typedef enum {
  VertexAttributeDimension_Position = 3,
  VertexAttributeDimension_Normal = 3,
  VertexAttributeDimension_Tangent = 4,
  VertexAttributeDimension_Color = 3,
  VertexAttributeDimension_Uv = 2,
} VertexAttributeDimension;

/*
  Cannonical structure of a vertex
 */
typedef struct {
  vertex_position position;
  vertex_normal normal;
  vertex_tangent tangent;
  vertex_color color;
  vertex_uv uv;
} Vertex;

void vertex_create(Vertex *);
Vertex vertex_from_array(float *);

void vertex_to_array(Vertex *, float *);
void vertex_copy(float *, float *);

#endif
