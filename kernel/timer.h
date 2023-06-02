#ifndef TIMER_H
#define TIMER_H

#include "util.h"

#define TIMER_TPS 2

void init_timer(u32 frequency);
u32 timer_get();

#endif