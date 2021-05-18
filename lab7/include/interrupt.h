#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#define CORE0_INTERRUPT_SOURCE (volatile unsigned int*)(0x40000060)
#define CORE0_TIMER_INTERUPPT_CONTROL (volatile unsigned int*)(0x40000040)
#define GPU_INTERRUPT_ROUTING (volatile unsigned int*)(0x4000000c)

#endif