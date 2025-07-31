#ifndef _VECTOR_UTILS_CORE_H_
#define _VECTOR_UTILS_CORE_H_

#include <cglm/cglm.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t VectorLength_t;

#define AXIS_COUNT 6

typedef enum {
  Axis_X,
  Axis_Y,
  Axis_Z,
  Axis_XY,
  Axis_YZ,
  Axis_XZ,
} Axis;

typedef enum {
  VectorLength_2 = 2,
  VectorLength_3 = 3,
  VectorLength_4 = 4,
} VectorLength;

typedef struct {
  vec2 a;
  vec2 b;
} vec2_pair;

typedef struct {
  ivec2 a;
  ivec2 b;
} ivec2_pair;

bool vec2_equal(vec2, vec2);
bool vec3_equal(vec3, vec3);
bool vec4_equal(vec4, vec4);

bool ivec2_equal(ivec2, ivec2);
bool ivec3_equal(ivec3, ivec3);
bool ivec4_equal(ivec4, ivec4);

void vec_baycentric(const vec2, const vec2, const vec2, const vec2,
                    const VectorLength, float *, float *, float *);

void ivec_to_vec(const int *, const VectorLength, float *);

void vec_world_axis(const Axis, vec3 *);

void vec3_replace_axis(vec3, vec3, const Axis, vec3 *);
#endif
