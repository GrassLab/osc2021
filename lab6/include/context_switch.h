#ifndef __CONTEXT_SWITCH_H_
#define __CONTEXT_SWITCH_H_

void switch_to(void *, void *);
void restore_regs_eret();

#endif