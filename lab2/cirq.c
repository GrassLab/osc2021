#include "include/irq.h"
#include "include/mini_uart.h"
#include "utils.h"

#define PBASE 0x3F000000
#define IRQ_BASIC_PENDING   (PBASE+0x0000B200)
#define IRQ_PENDING_1       (PBASE+0x0000B204)
#define IRQ_PENDING_2       (PBASE+0x0000B208)
#define FIQ_CONTROL         (PBASE+0x0000B20C)
#define ENABLE_IRQS_1       (PBASE+0x0000B210)
#define ENABLE_IRQS_2       (PBASE+0x0000B214)
#define ENABLE_BASIC_IRQS   (PBASE+0x0000B218)
#define DISABLE_IRQS_1      (PBASE+0x0000B21C)
#define DISABLE_IRQS_2      (PBASE+0x0000B220)
#define DISABLE_BASIC_IRQS  (PBASE+0x0000B224)

#define SYSTEM_TIMER_IRQ_0  (1 << 0)
#define SYSTEM_TIMER_IRQ_1  (1 << 1)
#define SYSTEM_TIMER_IRQ_2  (1 << 2)
#define SYSTEM_TIMER_IRQ_3  (1 << 3)

#define CORE0_INTERRUPT_SOURCE 0x40000060

void do_core_timer_handler(void)
{
    uart_send_string("From do_core_timer_handler\r\n");
    unsigned long seconds = core_timer_get_sec();
    uart_send_ulong(seconds);
    core_timer_handler();
}

void first_level_irq_handler(int type)
{
    switch (type) {
        case (2):
            do_core_timer_handler();
            break;
        default:
            uart_send_string("Unknown pending irq.\r\n");
    }
}

void second_level_irq_handler(int type)
{
    // switch (type) {
    //     case (SYSTEM_TIMER_IRQ_1):
    //         handle_timer_irq();
    //         break;
    //     default:
    //         uart_send_string("Unknown pending irq.\r\n");
    // }
    ;
}

void irq_handler(void)
{
    unsigned int second_level_irq_pend = get32(IRQ_PENDING_1);
    unsigned int first_level_irq_pend = get32(CORE0_INTERRUPT_SOURCE);

    /* Handle all pending first level irq. */
    for (int i = 0; i < 32; ++i)
        if (first_level_irq_pend & (1 << i))
            first_level_irq_handler(i+1);
            // uart_send_uint(first_level_irq_pend);

    /* Handle all pending second level irq. */
    for (int i = 0; i < 32; ++i)
        if (second_level_irq_pend & (1 << i))
            second_level_irq_handler(i);
}