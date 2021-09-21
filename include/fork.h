#ifndef _FORK_H
#define _FORK_H

#include "sched.h"

/*
 * PSR bits
 */
#define PSR_MODE_EL0t	0x00000000
#define PSR_MODE_EL1t	0x00000004
#define PSR_MODE_EL1h	0x00000005
#define PSR_MODE_EL2t	0x00000008
#define PSR_MODE_EL2h	0x00000009
#define PSR_MODE_EL3t	0x0000000c
#define PSR_MODE_EL3h	0x0000000d

struct pt_regs {
    unsigned long regs[31];
    unsigned long sp;
    unsigned long pc;
    unsigned long pstate;
};

int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long stack);
int move_to_user_mode(unsigned long pc);
int copy_process_virt(unsigned long clone_flags, unsigned long fn, unsigned long arg); // virtual address
int move_to_user_mode_virt(unsigned long pc, unsigned long user_start_address);
struct pt_regs *task_pt_regs(struct task_struct *tsk);
void _init_files_struct(struct task_struct *tsk);
long assignPID();

#endif