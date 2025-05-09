#include "math.h"
#include "../utils/system.h"

/**
   Return a number between 0 and 1 (0 excluded)
 */
float randf() {
  float r = rand();
  while (r == 0)
    r = rand();
  return (float)r / (float)RAND_MAX;
}

int clamp(const int v, const int min, const int max) {
  return MIN(MAX(v, min), max);
}

float clampf(const float v, const float min, const float max) {
  return MIN(MAX(v, min), max);
}
