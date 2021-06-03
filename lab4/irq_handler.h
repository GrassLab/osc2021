#ifndef __IRQ_HANDLER__
#define __IRQ_HANDLER__

#define MAX_IRQ_HANDLER 12

typedef void (*IRQ_Handler)(void);
IRQ_Handler irq_handlers[MAX_IRQ_HANDLER];
//void (*irq_handlers[MAX_IRQ_HANDLER])(void);

#endif