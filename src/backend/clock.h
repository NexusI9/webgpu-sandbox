#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <stdio.h>
#include <time.h>

typedef struct {
  double delta;
  clock_t current;
  clock_t last_time;
} cclock;

extern cclock g_clock;

void clock_init(cclock *);
void clock_update_delta(cclock *);

static inline double clock_delta(cclock *clock) { return clock->delta; }

#endif
