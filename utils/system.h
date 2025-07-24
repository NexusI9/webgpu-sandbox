#ifndef _SYSTEM_UTILS_H_
#define _SYSTEM_UTILS_H_

#include "../runtime/mesh/mesh.h"
#include <cglm/cglm.h>

#ifdef VERBOSE

#ifdef VERBOSE_LINE
#define PRINT_LINE(...) printf(" (%s:%d)\n", __FILE__, __LINE__)
#else
#define PRINT_LINE() printf("\n")
#endif

#define VERBOSE_PRINT(...)                                                     \
  do {                                                                         \
    printf("ℹ️ ");                                                              \
    printf(__VA_ARGS__);                                                       \
    PRINT_LINE();                                                              \
  } while (0)

#define VERBOSE_ERROR(...)                                                     \
  do {                                                                         \
    printf("🛑 ");                                                             \
    printf(__VA_ARGS__);                                                       \
    printf(" (%s:%d)\n", __FILE__, __LINE__);                                  \
  } while (0)

#define VERBOSE_WARNING(...)                                                   \
  do {                                                                         \
    printf("⚠️ ");                                                              \
    printf(__VA_ARGS__);                                                       \
    PRINT_LINE();                                                              \
  } while (0)

#define VERBOSE_DEBUG(...)                                                     \
  do {                                                                         \
    printf("🐞 ");                                                             \
    printf(__VA_ARGS__);                                                       \
    PRINT_LINE();                                                              \
  } while (0)

#define VERBOSE_SHADER_CREATE(...)                                             \
  do {                                                                         \
    printf("🎨 Creating shader: ");                                            \
    printf(__VA_ARGS__);                                                       \
    PRINT_LINE();                                                              \
  } while (0)

#define VERBOSE_MESH_BUILD(...)                                                \
  do {                                                                         \
    printf("🧱 Building mesh: ");                                              \
    printf(__VA_ARGS__);                                                       \
    PRINT_LINE();                                                              \
  } while (0)

#define VERBOSE_MESH_CREATE(...)                                               \
  do {                                                                         \
    printf("✨ Creating mesh: ");                                              \
    printf(__VA_ARGS__);                                                       \
    PRINT_LINE();                                                              \
  } while (0)

#define VERBOSE_IMPORT(...)                                                    \
  do {                                                                         \
    printf("📦 Importing: ");                                                  \
    printf(__VA_ARGS__);                                                       \
    PRINT_LINE();                                                              \
  } while (0)

#define VERBOSE_SUCCESS(...)                                                   \
  do {                                                                         \
    printf("☑️ ");                                                              \
    printf(__VA_ARGS__);                                                       \
    PRINT_LINE();                                                              \
  } while (0)

#define VERBOSE_PROCESS(...)                                                   \
  do {                                                                         \
    printf("⚙️ ");                                                              \
    printf(__VA_ARGS__);                                                       \
    PRINT_LINE();                                                              \
  } while (0)

#define VERBOSE_HEADER(...)                                                    \
  do {                                                                         \
    printf("===== ");                                                          \
    printf(__VA_ARGS__);                                                       \
    printf(" =====\n");                                                        \
  } while (0)

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
