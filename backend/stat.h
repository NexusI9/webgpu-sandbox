#ifndef _STAT_H_
#define _STAT_H_

#include <webgpu/webgpu.h>

#define STAT_COUNT_COUNT 4
typedef enum {
  StatCount_Vertex,
  StatCount_Texture,
  StatCount_Shader,
  StatCount_DrawCall,
} StatCount;

typedef struct {
  int count[STAT_COUNT_COUNT];
} Statistics;

#ifdef __cplusplus
extern "C" {
#endif

static inline void stat_init(Statistics *stat) { *stat = (Statistics){0}; }

static inline void stat_update_count(Statistics *stat, const StatCount type,
                                     int value) {
  stat->count[type] = value;
}

static inline int stat_get_count(Statistics *stat, const StatCount type) {
  return stat->count[type];
}

#ifdef __cplusplus
}
#endif

#endif
