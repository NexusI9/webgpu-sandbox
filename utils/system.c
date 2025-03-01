#include "system.h"
#include <stdio.h>

void print_vec3(vec3 vector) {
  printf("%f\t%f\t%f\n", vector[0], vector[1], vector[2]);
}

void print_vec2(vec2 vector) { printf("%f\t%f\n", vector[0], vector[1]); }

void print_vec4(vec4 vector) {
  printf("%f\t%f\t%f\t%f\n", vector[0], vector[1], vector[2], vector[3]);
}

void print_mat4(mat4 matrix) {
  for (int i = 0; i < 4; i++) {
    print_vec4(matrix[i]);
  }
}
