#ifndef ENTRY_H
#define ENTRY_H
#include "sched.h"

#define context_switch_size 0x400
void from_el1_to_el0 (u64);

void switch_to (void *current, void *next);

#endif
