#include "system.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void print_ivec3(const ivec3 vector) {
  printf("%d\t%d\t%d\n", vector[0], vector[1], vector[2]);
}

void print_ivec2(const ivec2 vector) { printf("%d\t%d\n", vector[0], vector[1]); }

void print_ivec4(const ivec4 vector) {
  printf("%d\t%d\t%d\t%d\n", vector[0], vector[1], vector[2], vector[3]);
}

void print_vec3(const vec3 vector) {
  printf("%f\t%f\t%f\n", vector[0], vector[1], vector[2]);
}

void print_vec2(const vec2 vector) { printf("%f\t%f\n", vector[0], vector[1]); }

void print_vec4(const vec4 vector) {
  printf("%f\t%f\t%f\t%f\n", vector[0], vector[1], vector[2], vector[3]);
}

void print_mat4(const mat4 matrix) {
  for (int i = 0; i < 4; i++) {
    print_vec4(matrix[i]);
  }
}

void print_list_float(float *list, size_t length, size_t stride) {

  for (int i = 0; i < length; i++) {

    printf("%f\t", list[i]);

    if ((i + 1) % stride == 0)
      printf("\n");
  }

  if (length % stride != 0)
    printf("\n");
}

void print_list_uint16(uint16_t *list, size_t length, size_t stride) {

  for (int i = 0; i < length; i++) {

    printf("%u\t", list[i]);

    /*if ((i + i) % stride == 0)
      printf("\n");*/
  }
  printf("\n");

  if (length % stride != 0)
    printf("\n");
}

void print_mesh_tree(mesh *mesh, uint16_t level) {

  for (uint16_t l = 0; l < level; l++)
    printf("\t");

  if (level != 0)
    printf("L ");

  printf("[%p] %s\n", mesh, mesh->name);

  level++;

  for (int i = 0; i < mesh->children.length; i++)
    print_mesh_tree(mesh->children.entries[i], level);
}
