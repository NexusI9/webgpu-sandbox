#ifndef _SYSTEM_UTILS_H_
#define _SYSTEM_UTILS_H_

#include <cglm/cglm.h>
#include "../runtime/mesh.h"

#ifdef VERBOSE
#define VERBOSE_PRINT(...) printf(__VA_ARGS__)
#else
#define VERBOSE_PRINT(...) // no-op
#endif


void print_ivec4(const ivec4);
void print_ivec3(const ivec3);
void print_ivec2(const ivec2);
void print_vec4(const vec4);
void print_vec3(const vec3);
void print_vec2(const vec2);
void print_mat4(const mat4);
void print_list_float(float *, size_t, size_t);
void print_list_uint16(uint16_t *, size_t, size_t);
void print_mesh_tree(mesh *, uint16_t);
#endif
