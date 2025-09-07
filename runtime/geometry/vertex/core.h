#ifndef _VERTEX_CORE_H_
#define _VERTEX_CORE_H_

#include <cglm/cglm.h>
#include <stddef.h>
#include <webgpu/webgpu.h>

#define VERTEX_STRIDE 14
#define VERTEX_ATTRIBUTE_COUNT 5

typedef enum {
  VertexStatus_Success,
  VertexStatus_AllocFail,
} VertexStatus;

typedef vec2 vertex_uv;
typedef vec3 vertex_color;
typedef vec3 vertex_position;
typedef vec3 vertex_normal;

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
  VertexAttributeOffset_Color = 9,
  VertexAttributeOffset_Uv = 12,
  VertexAttributeOffset_End = VERTEX_STRIDE,
} VertexAttributeOffset;

typedef enum {
  VertexAttributeDimension_Position = 3,
  VertexAttributeDimension_Normal = 3,
  VertexAttributeDimension_Tangent = 3,
  VertexAttributeDimension_Color = 3,
  VertexAttributeDimension_Uv = 2,
} VertexAttributeDimension;

/*
  Cannonical structure of a vertex
 */
typedef struct {
  vec3 position;
  vec3 normal;
  vec3 tangent;
  vec3 color;
  vec2 uv;
} Vertex;

void vertex_create(Vertex *);
Vertex vertex_from_array(float *);

void vertex_to_array(Vertex *, float *);
void vertex_copy(float *, float *);

#endif
