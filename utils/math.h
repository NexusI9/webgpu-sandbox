#ifndef _MATH_UTILS_H_
#define _MATH_UTILS_H_

#include "../resources/geometry/vertex.h"
#include <cglm/cglm.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define PI 3.14159265359

float randf();
int clamp(const int, const int min, const int max);
float clampf(const float, const float, const float max);

#endif
