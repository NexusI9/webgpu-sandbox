#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <stdio.h>
#include <time.h>

typedef struct {

  double delta;
  clock_t current;
  clock_t last_time;

} cclock;

cclock clock_create();
void clock_update_delta(cclock *);

#endif
