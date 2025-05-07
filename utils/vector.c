#include "vector.h"
#include <stdint.h>

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
