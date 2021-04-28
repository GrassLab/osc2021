#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define RUNQ_SIZE (100)

#include "thread.h"

void do_schedule();

int enqueue(struct Thread * t);
struct Thread * dequeue();

#endif