#ifndef _UTILS_MATRIX_H_
#define _UTILS_MATRIX_H_

#include <cglm/cglm.h>

typedef struct {
  float *position;
  float *target;
  vec3 *forward;
  vec3 *up;
  vec3 *right;
  vec3 *dest_position;
  mat4 *dest_matrix;
} UtilsMatrixLookatDescriptor;

void matrix_lookat(UtilsMatrixLookatDescriptor *);

void matrix_mesh_lookat(UtilsMatrixLookatDescriptor *);

#endif
