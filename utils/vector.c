#include "vector.h"
#include "system.h"
#include <stdint.h>
#include <webgpu/webgpu.h>

static bool vec_equal(float *, float *, uint8_t);

bool vec_equal(float *a, float *b, uint8_t length) {
  for (int i = 0; i < length; i++) {
    if (a[i] != b[i])
      return false;
  }

  return true;
}

bool vec2_equal(vec2 a, vec2 b) { return vec_equal(a, b, 2); }

bool vec3_equal(vec3 a, vec3 b) { return vec_equal(a, b, 3); }

bool vec4_equal(vec4 a, vec4 b) { return vec_equal(a, b, 4); }

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
