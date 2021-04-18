#include "exc.h"
#include "io.h"
#include "utility.h"
#include "mmio.h"

void print_el1_exc () {
    kprintf("spsr_el1: %x\n", get_spsr_el1());
    kprintf("elr_el1: %x\n", get_elr_el1());
    kprintf("esr_el1: %x\n\n", get_esr_el1());
}

#include "uart.h"
void irq_uart_handler () {
    push_read_buffer ();
}

void sp_elx_irq_handler () {
    u64 pending = *mmio(IRQ1_PENDING);
    pending |= (u64)(*mmio(IRQ2_PENDING)) << 32;
    switch (pending) {
        case IRQ_TABLE_AUX_INT:
            irq_uart_handler();
            return;
        case IRQ_TABLE_UART_INT:
        case IRQ_TABLE_SYSTEM_TIMER1:
        case IRQ_TABLE_SYSTEM_TIMER2:
        case IRQ_TABLE_USB_CONTROLLER:
        case IRQ_TABLE_I2C_SPI_SLV_INT:
        case IRQ_TABLE_PWA0:
        case IRQ_TABLE_PWA1:
        case IRQ_TABLE_SMI:
        case IRQ_TABLE_GPIO_INT0:
        case IRQ_TABLE_GPIO_INT1:
        case IRQ_TABLE_GPIO_INT2:
        case IRQ_TABLE_GPIO_INT3:
        case IRQ_TABLE_I2C_INT:
        case IRQ_TABLE_SPI_INT:
        case IRQ_TABLE_PCM_INT:
        default:
            kprintf("Error: Unsupport irq %x\n", pending);
    }
    while (1); /* trap */
}

int sys_call_handler () {
    u64 sys_num = get_x19();
    switch (sys_num) {
        /* show specific register */
        case 400:
            kprintf("spsr_el1: %x\n", get_spsr_el1());
            kprintf("elr_el1: %x\n", get_elr_el1());
            kprintf("esr_el1: %x\n\n", get_esr_el1());
            break;
        /* exit */
        case 60:
            return 60;
        /* read */
        case 0:
        /* write */
        case 1:
        /* open */
        case 2:
        default:
            kprintf("Unsupported syscall: %d\n", sys_num);
    }
    return 1;
}

void exc_error (int error) {
    switch (error) {
        case 0:
            kprintf("Unsupport exception: SP_EL0 sync\n");
            break;
        case 1:
            kprintf("Unsupport exception: SP_EL0 IRQ\n");
            break;
        case 2:
            kprintf("Unsupport exception: SP_EL0 FIQ\n");
            break;
        case 3:
            kprintf("Unsupport exception: SP_EL0 SError");
            break;
        case 4:
            kprintf("Unsupport exception: SP_ELx sync\n");
            break;
        case 5:
            kprintf("Unsupport exception: SP_ELx IRQ\n");
            break;
        case 6:
            kprintf("Unsupport exception: SP_ELx FIQ\n");
            break;
        case 7:
            kprintf("Unsupport exception: SP_ELx SError\n");
            break;
        case 8:
            kprintf("Unsupport exception: AArch64 sync\n");
            break;
        case 9:
            kprintf("Unsupport exception: AARch64 IRQ\n");
            break;
        case 10:
            kprintf("Unsupport exception: AARch64 FIQ\n");
            break;
        case 11:
            kprintf("Unsupport exception: AArch64 SError\n");
            break;
        case 12:
            kprintf("Unsupport exception: AArch32 sync\n");
            break;
        case 13:
            kprintf("Unsupport exception: AArch32 IRQ\n");
            break;
        case 14:
            kprintf("Unsupport exception: AArch32 FIQ\n");
            break;
        case 15:
            kprintf("Unsupport exception: AArch32 SError\n");
            break;
        default:
            kprintf("Unkown exception: %d\n", error);
    }

    /* trap */
    while (1) ;
}

/* IRQ table */
#define system_timer_match1 (1 << 1)

void enable_irq_system_timer1 () {
    *mmio(ENABLE_IRQS1) = system_timer_match1;
}

