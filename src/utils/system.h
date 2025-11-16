#ifndef _SYSTEM_UTILS_H_
#define _SYSTEM_UTILS_H_

#include <cglm/cglm.h>
#include <stdio.h>
#include <time.h>
#include <cglm/types.h>
#include <stdint.h>

#include "runtime/mesh/mesh.h"
#include "runtime/mesh/core.h"

/*

 DEBUG MALLOC

 */

#ifdef DEBUG_MALLOC

// Declarations for our custom functions
void *custom_malloc(size_t size, const char *file, int line);
void custom_free(void *ptr, const char *file, int line);

// The macros that replace malloc and free
#define malloc(size) custom_malloc(size, __FILE__, __LINE__)
#define free(ptr) custom_free(ptr, __FILE__, __LINE__)

#endif // DEBUG_MALLOC

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
void print_bin(size_t const, void const *const);

#endif
