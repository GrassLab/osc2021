#ifndef __IRQ_H__
#define __IRQ_H__
#include "gpio.h"
#include "utils.h"
#include "timer.h"
#include "uart.h"

extern void core_timer_disable(void);
extern void core_timer_handler();
extern void enable_irq();
extern void disable_irq();
extern void sync_call_uart();

void handle_sync_el1(unsigned long, unsigned long);
void handle_el1_irq();
void handle_el0_irq();
void el1_timer_irq();
void el0_timer_irq();
#define IRQ_PENDING_1 (MMIO_BASE + 0x0000B204)
#define ENB_IRQS1 (MMIO_BASE + 0x0000B210)
#define DISABLE_IRQS1 (MMIO_BASE + 0x0000B21C)
#define AUX_IRQ (1 << 29)
#define CORE0_INTERRUPT_SOURCE 0x40000060

#endif