#ifndef UTILITY_H
#define UTILITY_H
#include "data_type.h"
extern u64 get_spsr_el1 ();
extern u64 get_elr_el1 ();
extern u64 get_esr_el1 ();
extern u64 get_x0 ();
extern u64 get_x30 ();
extern u64 get_x19 ();
extern u64 get_fp ();
extern u64 get_sp ();
extern u64 get_lr ();
extern u64 get_new_task_entry ();

extern void set_cntp_tval_el0 (u64 value);
extern void set_cntp_ctl_el0 (u64 value);
extern void set_spsr_el1 (u64 value);
extern void set_sp_el0 (u64 value);
extern void set_sp (u64 value);
extern void enable_DAIF_irq ();
extern void disable_DAIF_irq ();

extern u64 time_counter ();
extern u64 time_freq ();
extern unsigned long get__start ();
extern void jump_address (unsigned long address);
extern void from_el1_to_el0 ();
extern void tmp_p ();

#endif
