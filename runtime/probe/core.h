#ifndef _PROBE_CORE_H_
#define _PROBE_CORE_H_

#include <stdint.h>

typedef struct {
  uint32_t reflection_grid;
  uint32_t reflection_plane;
  uint32_t irradiance;
  uint32_t _pad;
} ProbeCountUniform;

#endif
