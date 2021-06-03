#ifndef __IRQ__
#define __IRQ__

#define MAX_IRQ_HANDLER 12

typedef void (*IRQ_Handler)(void);
IRQ_Handler irq_handlers[MAX_IRQ_HANDLER];

#define  CORE0_INTERRUPT_SOURCE 0x40000060
void el0_irq_handler();
void el1_irq_handler();

#endif