#ifndef _SYSTEM_UTILS_H_
#define _SYSTEM_UTILS_H_

#include "../runtime/mesh.h"
#include <cglm/cglm.h>

#ifdef VERBOSE
#define VERBOSE_PRINT(...) printf(__VA_ARGS__)
#define VERBOSE_ERROR(...) perror(__VA_ARGS__)
#define VERBOSE_WARNING(...) printf(__VA_ARGS__)
#else
#define VERBOSE_PRINT(...) // no-op
#define VERBOSE_ERROR(...)
#define VERBOSE_WARNING(...)
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
void print_list_uint32(uint32_t *, size_t, size_t);
void print_mesh_tree(Mesh *, uint16_t);
#endif
