#ifndef _SYSTEM_UTILS_H_
#define _SYSTEM_UTILS_H_

#include "../runtime/mesh/mesh.h"
#include <cglm/cglm.h>
#include <stdio.h>
#include <time.h>

#ifdef VERBOSE

#ifdef VERBOSE_LINE
#define PRINT_LINE(...) printf(" (%s:%d)\n", __FILE__, __LINE__)
#else
#define PRINT_LINE() printf("\n")
#endif

#define VERBOSE_PRINT(...)                                                     \
  do {                                                                         \
    printf(__VA_ARGS__);                                                       \
    PRINT_LINE();                                                              \
  } while (0)
#define VERBOSE_INFO(...)                                                      \
  do {                                                                         \
    printf("ℹ️ ");                                                              \
    printf(__VA_ARGS__);                                                       \
    PRINT_LINE();                                                              \
  } while (0)

#define VERBOSE_ERROR(...)                                                     \
  do {                                                                         \
    printf("🛑 ");                                                             \
    printf(__VA_ARGS__);                                                       \
    PRINT_LINE();                                                              \
  } while (0)

#define VERBOSE_WARNING(...)                                                   \
  do {                                                                         \
    printf("⚠️ ");                                                              \
    printf(__VA_ARGS__);                                                       \
    PRINT_LINE();                                                              \
  } while (0)

#define VERBOSE_DEBUG(...)                                                     \
  do {                                                                         \
    printf("");                                                                \
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
    printf("🔼 Creating mesh: ");                                              \
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
#define VERBOSE_INFO(...)
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
#endif // VERBOSE

/*

  DEBUG_TIME

 */

#ifdef DEBUG_TIME

#define TIMER(name, code)                                                      \
  do {                                                                         \
    struct timespec _start, _end;                                              \
    clock_gettime(CLOCK_MONOTONIC, &_start);                                   \
    code clock_gettime(CLOCK_MONOTONIC, &_end);                                \
    double _elapsed = (_end.tv_sec - _start.tv_sec) * 1000.0 +                 \
                      (_end.tv_nsec - _start.tv_nsec) / 1000000.0;             \
    printf("%s > %.3f ms\n", name, _elapsed);                                  \
  } while (0)

#else
#define TIMER(name, code)                                                      \
  do {                                                                         \
    code                                                                       \
  } while (0)
#endif

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
