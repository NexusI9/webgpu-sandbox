#ifndef _SYSTEM_UTILS_H_
#define _SYSTEM_UTILS_H_

#include "../include/cglm/mat4.h"
#include "../include/cglm/vec2.h"
#include "../include/cglm/vec3.h"
#include "../include/cglm/vec4.h"
#include "../runtime/mesh.h"

void print_vec4(vec4);
void print_vec3(vec3);
void print_vec2(vec2);
void print_mat4(mat4);
void print_list_float(float *, size_t, size_t);
void print_list_uint16(uint16_t *, size_t, size_t);
void print_mesh_tree(mesh *, uint16_t);
#endif
