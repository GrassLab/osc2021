#ifndef __INTERRUPT_ENTRY__
#define __INTERRUPT_ENTRY__

#define  CORE0_INTERRUPT_SOURCE 0x40000060

void invalid_entry();
void el0_sync_entry();
void irq_entry();

#endif