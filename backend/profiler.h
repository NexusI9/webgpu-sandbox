#ifndef _PROFILER_H_
#define _PROFILER_H_

#include <stdint.h>
#include <time.h>
#include "utils/defines.h"

#define PROFILER_LATENCY_TYPE_COUNT 7
typedef enum {
  ProfilerLatencyType_ShadowPass,
  ProfilerLatencyType_ReflectionPass,
  ProfilerLatencyType_UIPass,
  ProfilerLatencyType_MainLoop,
  ProfilerLatencyType_BlitPass,
  ProfilerLatencyType_KawasePass,
  ProfilerLatencyType_BloomPass,
} ProfilerLatencyType;

typedef struct {
  struct timespec start;
  struct timespec end;
} ProfilerLatency;

typedef struct {
  ProfilerLatency latencies[PROFILER_LATENCY_TYPE_COUNT];
} Profiler;

EXTERN_C_BEGIN

static inline void profiler_init(Profiler *profiler) {
  *profiler = (Profiler){0};
}

static inline void profiler_latency_start(Profiler *profiler,
                                          const ProfilerLatencyType type) {
  clock_gettime(CLOCK_MONOTONIC, &profiler->latencies[type].start);
}

static inline void profiler_latency_end(Profiler *profiler,
                                        const ProfilerLatencyType type) {
  clock_gettime(CLOCK_MONOTONIC, &profiler->latencies[type].end);
}

static inline void profiler_latency_clear(Profiler *profiler,
                                          const ProfilerLatencyType type) {
  profiler->latencies[type] = (ProfilerLatency){0};
}

static inline void profiler_latency_clear_all(Profiler *profiler) {
  for (uint8_t i = 0; i < PROFILER_LATENCY_TYPE_COUNT; i++)
    profiler->latencies[i] = (ProfilerLatency){0};
}

static inline double
profiler_latency_get_elapsed(Profiler *profiler,
                             const ProfilerLatencyType type) {

  struct timespec *start = &profiler->latencies[type].start;
  struct timespec *end = &profiler->latencies[type].end;

  return (end->tv_sec - start->tv_sec) * 1000.0 +
         (end->tv_nsec - start->tv_nsec) / 1000000.0;
}

EXTERN_C_END

#endif
