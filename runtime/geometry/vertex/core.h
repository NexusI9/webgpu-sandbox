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
  If change is required, make sure to adjust the meshbin file indexes as
  they are u32 based as well.
 */
typedef uint32_t vindex_t;
typedef float vattr_t;

/*
  Cannonical structure of a vertex
 */
typedef struct {
  vec3 position;
  vec3 normal;
  vec3 color;
  vec2 uv;
} Vertex;

typedef struct {
  vattr_t *entries;
  size_t length;
  size_t capacity;
  WGPUBuffer buffer;
} VertexAttribute;

typedef struct {
  vindex_t *entries;
  size_t length;
  size_t capacity;
  WGPUBuffer buffer;
} VertexIndex;

typedef enum {
  VertexAttributeName_Undefined = 0,
  VertexAttributeName_Position = 1 << 0,
  VertexAttributeName_Normal = 1 << 1,
  VertexAttributeName_Color = 1 << 2,
  VertexAttributeName_Uv = 1 << 3,
  VertexAttributeName_All = 1 << 4,
} VertexAttributeName;

/*
  List of mesh vertex attributes and index
 */
typedef struct {

  float *position;
  float *normal;
  float *color;
  float *uv;

  vindex_t *index;
  size_t count;

} VertexList; // TODO: unsure about the naming..

void vertex_create(Vertex *);
void vertex_list_create(VertexList *, size_t);
Vertex vertex_from_array(float *);
void vertex_find_equal_attr(Vertex *, VertexAttribute *, VertexAttributeName,
                            VertexAttribute *);
void vertex_to_array(Vertex *, float *);
void vertex_copy(float *, float *);

void vertex_index_print(VertexIndex *);
int vertex_index_copy(VertexIndex *, VertexIndex *);
void vertex_index_destroy(VertexIndex *);

void vertex_attribute_print(VertexAttribute *);
void vertex_attribute_set_color(VertexAttribute *, vertex_color *);
void vertex_attribute_set_uv(VertexAttribute *, vertex_uv *);
int vertex_attribute_copy(VertexAttribute *, VertexAttribute *);
void vertex_attribute_destroy(VertexAttribute *);


#endif
