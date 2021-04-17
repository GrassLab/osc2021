#ifndef UTILITY_H
#define UTILITY_H
#include "data_type.h"
extern u64 get_spsr_el1 ();
extern u64 get_elr_el1 ();
extern u64 get_esr_el1 ();
extern u64 get_x0 ();
extern u64 get_x30 ();
extern u64 get_x19 ();
extern void enable_DAIF_irq ();
extern void disable_DAIF_irq ();

extern unsigned long long time_counter ();
extern unsigned int time_freq ();
extern unsigned long get__start ();
extern void jump_address (unsigned long address);
extern void from_el1_to_el0 ();
extern void tmp_p ();

#endif
