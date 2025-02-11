#include "clock.h"
#include <time.h>

cclock clock_create(){
    cclock c;
    c.last_time = clock();
    c.delta = 0.0;

    return c;
}

void clock_update_delta(cclock * c){
    clock_t current_time = clock();
    c->delta = (double)(current_time - c->last_time) / CLOCKS_PER_SEC;
    c->last_time = current_time;
}
