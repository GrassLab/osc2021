#include "exc.h"
#include "io.h"
#include "utility.h"
#include "mmio.h"
#include "sched.h"
#include "uart.h"

void print_el1_exc () {
    kprintf("spsr_el1: %x\n", get_spsr_el1());
    kprintf("elr_el1: %x\n", get_elr_el1());
    kprintf("esr_el1: %x\n\n", get_esr_el1());
}

void set_timer1 (float second) {
    u64 value = second * time_freq();
    set_cntp_tval_el0(value);
}

#include "uart.h"
void irq_uart_handler () {
    push_read_buffer ();
}

void irq_timer1_handler () {
    // TODO reduce time window
    set_timer1(0.01);
    schedule();
}

#define CNTPNSIRQ_INTERRUPT 0x2
void sp_elx_irq_handler () {
    u32 core0_source = *mmio(CORE0_INTERRUPT_SOURCE);
    /* first level interrupt */
    switch (core0_source) {
        case CNTPNSIRQ_INTERRUPT:
            kprintf("into sp_elx\n");
            irq_timer1_handler();
            return;
    }

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
            kprintf("Error: sp_elx unsupport irq %x\n", pending);
    }
    while (1); /* trap */
}

int sys_call_handler (struct trap_frame *tf) {
    u64 sys_num = tf->x0;
    switch (sys_num) {
        /* show specific register */
        case 400:
            kprintf("spsr_el1: %x\n", get_spsr_el1());
            kprintf("elr_el1: %x\n", get_elr_el1());
            kprintf("esr_el1: %x\n\n", get_esr_el1());
            break;

        /* yield */
        case 401:
            schedule();
            return 0;

        /* create thread */
        case 402:
            create_thread(tf, (void *)(tf->x1));
            return 0;

        /* stack info */
        case 403:
        {
            *(void **)(tf->x1) = current_task->stack_top;
            *(void **)(tf->x2) = current_task->kstack_top;
            *(void **)(tf->x3) = (void *)(tf->sp);
            return 0;
        }

        /* read */
        case 0:
            read_line((char *)(tf->x1), tf->x2);
            return 0;

        /* write */
        case 1:
            kprintf("%s", (char *)(tf->x1));
            return 0;

        /* sleep */
        case 35:
        {
            schedule_wait(tf->x1);
            return 0;
        }

        /* get pid */
        case 39:
            _get_pid(tf);
            return 0;
        /* fork */
        case 57:
            fork_thread(tf);
            return 0;

        /* exec */
        case 59:
            exec_thread_cpio(tf);
            return 0;

        /* exit */
        case 60:
            schedule_kill();
            return 60;

        default:
            kprintf("Unsupported syscall: %d\n", sys_num);
    }
    return 1;
}

void aarch64_irq_handler () {
    u32 core0_source = *mmio(CORE0_INTERRUPT_SOURCE);
    /* first level interrupt */
    switch (core0_source) {
        case CNTPNSIRQ_INTERRUPT:
            irq_timer1_handler();
            return;
    }

    /* second level interrupt */
    u64 pending = *mmio(IRQ1_PENDING);
    pending |= (u64)(*mmio(IRQ2_PENDING)) << 32;
    u32 basic_pending = *mmio(IRQ_BASIC_PENDING);
    kprintf("basic: %x\n", basic_pending);
    kprintf("irq1: %x\n", *mmio(IRQ1_PENDING));
    kprintf("irq2: %x\n", *mmio(IRQ2_PENDING));
    switch (pending) {
        case IRQ_TABLE_AUX_INT:
            irq_uart_handler();
            return;
        case IRQ_TABLE_SYSTEM_TIMER1:
            irq_timer1_handler();
            return;
        case IRQ_TABLE_UART_INT:
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
            kprintf("Error: aarch64 unsupport irq %x\n", pending);
    }
    while (1); /* trap */
}

void exc_error (int error, struct trap_frame *tf) {
    kprintf("spsr_el1: %x\n", get_spsr_el1());
    kprintf("elr_el1: %x\n", get_elr_el1());
    kprintf("esr_el1: %x\n\n", get_esr_el1());
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

    kprintf("x0: %x\n", tf->x0);
    kprintf("x1: %x\n", tf->x1);
    kprintf("x2: %x\n", tf->x2);
    kprintf("x3: %x\n", tf->x3);
    kprintf("x4: %x\n", tf->x4);
    kprintf("x5: %x\n", tf->x5);
    kprintf("x6: %x\n", tf->x6);
    kprintf("sp: %x\n", tf->sp);
    kprintf("lr: %x\n", tf->lr);

    /* trap */
    while (1) ;
}

/* IRQ table */
#define system_timer_match1 (1 << 1)

void enable_irq_system_timer1 () {
    *mmio(ENABLE_IRQS1) = system_timer_match1;
}

void enable_core_timer () {
    set_cntp_ctl_el0(1); /* enable */
    u64 freq = time_freq();
    set_cntp_tval_el0(freq);
    set_spsr_el1(0x340);
}

