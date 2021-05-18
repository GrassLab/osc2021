#ifndef	_EXCE_H
#define	_EXCE_H

#include <base.h>

#define CORE0_IRQ_SRC       ((volatile unsigned int*)0x40000060)
// To identify IRQ interrupt source is from AUX
// // GPU pending 1 register, see p.115 in BCM2835 ARM Peripherals docs
#define IRQ_PENDING_1       ((volatile unsigned int*)(MMIO_BASE+0x0000B204))
// For miniUART interrupt. 
// To Enable second level interrupt controller’s IRQs1(0x3f00b210)’s bit29. 
// See p.116 in BCM2835 ARM Peripherals docs
#define ENABLE_IRQS_1       ((volatile unsigned int*)(MMIO_BASE+0x0000B210))
// To Disable second level interrupt controller’s IRQs1(0x3f00b210)’s bit29. 
#define DISABLE_IRQS_1		((volatile unsigned int*)(PBASE+0x0000B21C))
// enable/disable/check pending miniUART interrupt bit 29
// See p.113 ARM peripherals interrupts table in BCM2835 docs
#define AUX_IRQ (1 << 29) 

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address);
void sync_exc_router(unsigned long spsr, unsigned long elr, unsigned long esr);
void sync_svc_handler(unsigned long spsr, unsigned long elr, unsigned long esr);
void irq_exc_router();
void print_pstate_interrupt_mask_bits();

// defined in entry.S. 
// It's weird that we can't define them in entry.h. It seems that because we include entry.h in entry.S
// void enable_irq_persist();
// void disable_irq_persist();
// void enable_irq();
// void disable_irq();
// void enable_interrupt();
// void disable_interrupt();

#endif  /*_EXC_H */

