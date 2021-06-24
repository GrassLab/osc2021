#ifndef XCPT_FUNC_H
#define XCPT_FUNC_H

#ifndef __ASSEMBLER__

void from_el1_to_el0(void);
void core_timer_enable(void);
void run_user_program(unsigned long start, unsigned long stack_top);

#endif
#endif