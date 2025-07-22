#ifndef _SYSTEM_UTILS_H_
#define _SYSTEM_UTILS_H_

#include "../runtime/mesh/mesh.h"
#include <cglm/cglm.h>

#ifdef VERBOSE
#define VERBOSE_PRINT(...)                                                     \
  printf("ℹ️ ");                                                                \
  printf(__VA_ARGS__);                                                         \
  printf(" (%s:%d)\n", __FILE__, __LINE__)
#define VERBOSE_ERROR(...)                                                     \
  fprintf(stderr, "❌ ");                                                      \
  fprintf(stderr, __VA_ARGS__);                                                \
  printf(" (%s:%d)\n", __FILE__, __LINE__)
#define VERBOSE_WARNING(...)                                                   \
  printf("⚠️ ");                                                                \
  printf(__VA_ARGS__);                                                         \
  printf(" (%s:%d)\n", __FILE__, __LINE__)
#define VERBOSE_DEBUG(...)                                                     \
  printf("🐞 ");                                                               \
  printf(__VA_ARGS__);                                                         \
  printf(" (%s:%d)\n", __FILE__, __LINE__)
#define VERBOSE_SHADER_CREATE(...)                                             \
  printf("🎨 Creating shader: ");                                              \
  printf(__VA_ARGS__);                                                         \
  printf(" (%s:%d)\n", __FILE__, __LINE__)
#define VERBOSE_MESH_BUILD(...)                                                \
  printf("🧱 Building mesh: ");                                                \
  printf(__VA_ARGS__);                                                         \
  printf(" (%s:%d)\n", __FILE__, __LINE__)
#define VERBOSE_MESH_CREATE(...)                                               \
  printf("✨ Creating mesh: ");                                                \
  printf(__VA_ARGS__);                                                         \
  printf(" (%s:%d)\n", __FILE__, __LINE__)
#define VERBOSE_IMPORT(...)                                                    \
  printf("📦 Importing: ");                                                    \
  printf(__VA_ARGS__);                                                         \
  printf(" (%s:%d)\n", __FILE__, __LINE__)
#define VERBOSE_SUCCESS(...)                                                   \
  printf("☑️ ");                                                                \
  printf(__VA_ARGS__);                                                         \
  printf(" (%s:%d)\n", __FILE__, __LINE__)
#define VERBOSE_PROCESS(...)                                                   \
  printf("⚙️ ");                                                                \
  printf(__VA_ARGS__);                                                         \
  printf(" (%s:%d)\n", __FILE__, __LINE__)
#define VERBOSE_HEADER(...)                                                    \
  printf("===== ");                                                            \
  printf(__VA_ARGS__);                                                         \
  printf(" =====\n")
#else
#define VERBOSE_PRINT(...) // no-op
#define VERBOSE_ERROR(...)
#define VERBOSE_WARNING(...)
#define VERBOSE_DEBUG(...)
#define VERBOSE_SHADER_CREATE(...)
#define VERBOSE_MESH_BUILD(...)
#define VERBOSE_MESH_CREATE(...)
#define VERBOSE_IMPORT(...)
#define VERBOSE_SUCCESS(...)
#define VERBOSE_PROCESS(...)
#define VERBOSE_HEADER(...)
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
