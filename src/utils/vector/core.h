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
  Axis_XYZ,
  Axis_View,
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

static inline bool vec_equal(float *a, float *b, uint8_t length) {
  for (int i = 0; i < length; i++) {
    if (a[i] != b[i])
      return false;
  }

  return true;
}

static inline bool ivec_equal(int *a, int *b, uint8_t length) {
  for (int i = 0; i < length; i++) {
    if (a[i] != b[i])
      return false;
  }

  return true;
}

static inline bool vec2_equal(vec2 a, vec2 b) {
  return vec_equal(a, b, VectorLength_2);
}
static inline bool vec3_equal(vec3 a, vec3 b) {
  return vec_equal(a, b, VectorLength_3);
}
static inline bool vec4_equal(vec4 a, vec4 b) {
  return vec_equal(a, b, VectorLength_4);
}
static inline bool ivec2_equal(ivec2 a, ivec2 b) {
  return ivec_equal(a, b, VectorLength_2);
}
static inline bool ivec3_equal(ivec3 a, ivec3 b) {
  return ivec_equal(a, b, VectorLength_3);
}
static inline bool ivec4_equal(ivec4 a, ivec4 b) {
  return ivec_equal(a, b, VectorLength_4);
}

/**
   Compute baycentric weights {u,v,w} of a point between 3 others
 */
static inline void vec_baycentric(const vec2 A, const vec2 B, const vec2 C,
                                  const vec2 P, const VectorLength length,
                                  float *u, float *v, float *w) {

  float v0[length], v1[length], v2[length];
  float d00, d01, d11, d20, d21 = 0.0f;
  int i;

  for (i = 0; i < length; i++) {
    v0[i] = B[i] - A[i];
    v1[i] = C[i] - A[i];
    v2[i] = P[i] - A[i];

    d00 += v0[i] * v0[i];
    d01 += v0[i] * v1[i];
    d11 += v1[i] * v1[i];
    d20 += v2[i] * v0[i];
    d21 += v2[i] * v1[i];
  }

  float denom = d00 * d11 - d01 * d01;

  if (denom == 0.0f)
    // TODO: error handling
    denom = 1.0f;

  *v = (d11 - d20 - d01 * d21) / denom;
  *w = (d00 * d21 - d01 * d20) / denom;
  *u = 1.0f - *v - *w;
}

static inline void ivec_to_vec(const int *src, const VectorLength length,
                               float *dest) {
  for (VectorLength_t l = 0; l < length; l++)
    dest[l] = (float)src[l];
}

static inline void vec_world_axis(const Axis axis, vec3 *dest) {

  vec3 axis_dir[] = {
      [Axis_X] = {1.0f, 0.0f, 0.0f},   // X axis
      [Axis_Y] = {0.0f, 1.0f, 0.0f},   // Y axis
      [Axis_Z] = {0.0f, 0.0f, 1.0f},   // Z axis
      [Axis_XY] = {1.0f, 0.0f, 0.0f},  // Z normal
      [Axis_YZ] = {0.0f, 1.0f, 0.0f},  // X normal
      [Axis_XZ] = {0.0f, 0.0f, 1.0f},  // Y normal
      [Axis_XYZ] = {1.0f, 1.0f, 1.0f}, // XYZ combined
      [Axis_View] = {0}                // Unvalid
  };

  glm_vec3_copy(axis_dir[axis], *dest);
}

/**
   Replace the values of src vector depending on given axis:
   - if axis is X, replace src.x by value.x.
   - if axis is XY, replace src.x and src.y by value.x and value.y.
 */

static const vec3 axis_factor[] = {
    // 1D axis
    [Axis_X] = {1.0f, 0.0f, 0.0f},
    [Axis_Y] = {0.0f, 1.0f, 0.0f},
    [Axis_Z] = {0.0f, 0.0f, 1.0f},
    // 2D Axis
    [Axis_XY] = {1.0f, 1.0f, 0.0f},
    [Axis_YZ] = {0.0f, 1.0f, 1.0f},
    [Axis_XZ] = {1.0f, 0.0f, 1.0f},
    // 3D Axis
    [Axis_XYZ] = {1.0f, 1.0f, 1.0f},
    // Need to be calculated based on camera direction
    [Axis_View] = {1.0f, 1.0f, 1.0f},
};

static inline void vec3_replace_axis(vec3 src, vec3 value, const Axis axis,
                                     vec3 *dest) {

  // first copy src to destination
  glm_vec3_copy(src, *dest);

  //  replace by value if the axis factor is 1
  for (size_t i = 0; i < VectorLength_3; i++)
    (*dest)[i] = (axis_factor[axis][i] > 0.0f) ? value[i] : (*dest)[i];
}

static inline float vec3_max_value(vec3 src) {
  float max = src[0];
  for (size_t i = 1; i < VectorLength_3; i++)
    max = glm_max(max, src[i]);

  return max;
}

static inline float vec3_max_abs_value(vec3 src) {
  float max = fabs(src[0]);
  for (size_t i = 1; i < VectorLength_3; i++)
    max = glm_max(max, fabs(src[i]));

  return max;
}

static inline void vec3_reflect_point(vec3 pos, vec3 n, float d, vec3 dest) {
  vec3 scaled_n;
  glm_vec3_scale(n, 2.0f * glm_dot(n, pos) - d, scaled_n);
  glm_vec3_sub(pos, scaled_n, dest);
}

static inline void vec3_reflect_dir(vec3 dir, vec3 n, vec3 dest) {
  vec3 scaled_n;
  glm_vec3_scale(n, 2.0f * glm_dot(n, dir), scaled_n);
  glm_vec3_sub(dir, scaled_n, dest);
}

static inline void vec3_tangent(vec3 n, vec3 dest) {

  vec3 up;
  if (fabsf(n[1]) < 0.999f)
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, up);
  else
    glm_vec3_copy((vec3){1.0f, 0.0f, 0.0f}, up);

  glm_vec3_cross(n, up, dest);
  glm_normalize(dest);
}

static inline void vec3_bitangent(vec3 n, vec3 t, vec3 dest) {
  glm_vec3_cross(n, t, dest);
  glm_vec3_normalize(dest);
}

static inline void vec2_avg(const vec2 a, const vec2 b, vec2 dest) {
  dest[0] = (a[0] + b[0]) / 2;
  dest[1] = (a[1] + b[1]) / 2;
}

#endif
