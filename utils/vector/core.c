#include "core.h"
#include "../system.h"
#include <stdint.h>
#include <webgpu/webgpu.h>

static inline bool vec_equal(float *, float *, VectorLength_t);
static inline bool ivec_equal(int *, int *, VectorLength_t);

bool vec_equal(float *a, float *b, uint8_t length) {
  for (int i = 0; i < length; i++) {
    if (a[i] != b[i])
      return false;
  }

  return true;
}

bool ivec_equal(int *a, int *b, uint8_t length) {
  for (int i = 0; i < length; i++) {
    if (a[i] != b[i])
      return false;
  }

  return true;
}

bool vec2_equal(vec2 a, vec2 b) { return vec_equal(a, b, VectorLength_2); }

bool vec3_equal(vec3 a, vec3 b) { return vec_equal(a, b, VectorLength_3); }

bool vec4_equal(vec4 a, vec4 b) { return vec_equal(a, b, VectorLength_4); }

bool ivec2_equal(ivec2 a, ivec2 b) { return ivec_equal(a, b, VectorLength_2); }

bool ivec3_equal(ivec3 a, ivec3 b) { return ivec_equal(a, b, VectorLength_3); }

bool ivec4_equal(ivec4 a, ivec4 b) { return ivec_equal(a, b, VectorLength_4); }

/**
   Compute baycentric weights {u,v,w} of a point between 3 others
 */
void vec_baycentric(const vec2 A, const vec2 B, const vec2 C, const vec2 P,
                    const VectorLength length, float *u, float *v, float *w) {

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

void ivec_to_vec(const int *src, const VectorLength length, float *dest) {
  for (VectorLength_t l = 0; l < length; l++)
    dest[l] = (float)src[l];
}

void vec_world_axis(const Axis axis, vec3 *dest) {

  vec3 axis_dir[] = {
      [Axis_X] = {1.0f, 0.0f, 0.0f},   // X axis
      [Axis_Y] = {0.0f, 1.0f, 0.0f},   // Y axis
      [Axis_Z] = {0.0f, 0.0f, 1.0f},   // Z axis
      [Axis_XY] = {0.0f, 0.0f, 1.0f},  // Z normal
      [Axis_YZ] = {1.0f, 0.0f, 0.0f},  // X normal
      [Axis_XZ] = {0.0f, 1.0f, 0.0f},  // Y normal
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

void vec3_replace_axis(vec3 src, vec3 value, const Axis axis, vec3 *dest) {

  // first copy src to destination
  glm_vec3_copy(src, *dest);

  //  replace by value if the axis factor is 1
  for (size_t i = 0; i < VectorLength_3; i++)
    (*dest)[i] = (axis_factor[axis][i] > 0.0f) ? value[i] : (*dest)[i];
}

float vec3_max_value(vec3 src) {
  float max = src[0];
  for (size_t i = 1; i < VectorLength_3; i++)
    max = glm_max(max, src[i]);

  return max;
}

float vec3_max_abs_value(vec3 src) {
  float max = fabs(src[0]);
  for (size_t i = 1; i < VectorLength_3; i++)
    max = glm_max(max, fabs(src[i]));

  return max;
}
