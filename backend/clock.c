#include "clock.h"
#include <time.h>


cclock g_clock = {0};

void clock_init(cclock * c){
    c->last_time = clock();
    c->delta = 0.0;
}

void clock_update_delta(cclock * c){
    clock_t current_time = clock();
    c->delta = (double)(current_time - c->last_time) / CLOCKS_PER_SEC;
    c->last_time = current_time;
}
