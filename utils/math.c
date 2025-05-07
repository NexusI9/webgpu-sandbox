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
