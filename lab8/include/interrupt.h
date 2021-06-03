#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#define ARM_LOCAL_BASE 0xffff000040000000
#define CORE0_INTERRUPT_SOURCE (volatile unsigned int*)(ARM_LOCAL_BASE + 0x60)
#define CORE0_TIMER_INTERUPPT_CONTROL (volatile unsigned int*)(ARM_LOCAL_BASE + 0x40)
#define GPU_INTERRUPT_ROUTING (volatile unsigned int*)(ARM_LOCAL_BASE + 0xc)

#endif