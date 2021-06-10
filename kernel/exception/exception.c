#include "exception.h"
#include "time.h"
#include "../process/process.h"
#include "../../lib/uart.h"

void log_state()
{
    unsigned long esr, elr, spsr;
    asm volatile("mrs %0, esr_el1 ":"=r"(esr));
    asm volatile("mrs %0, elr_el1 ":"=r"(elr));
    asm volatile("mrs %0, spsr_el1":"=r"(spsr));

    uart_puts("--------------- State ---------------\n");
    uart_puts("SPSR: ");
    uart_puts_h(spsr);
    uart_puts("\n");
    uart_puts("ELR: ");
    uart_puts_h(elr);
    uart_puts("\n");
    uart_puts("ESR: ");
    uart_puts_h(esr);
    uart_puts("\n");
    uart_puts("-------------------------------------\n");
}

void handle_exception(struct Trap_Frame * tf)
{
    unsigned long esr = 0, svc = 0;
    asm volatile("mrs %0, esr_el1":"=r"(esr));

    if (((esr >> 26) & 0x3f) == 0x15)
    {
        svc = esr & 0x1FFFFFF;
    }
    else
    {
        uart_puts("Synchronous exception\n");
        log_state();
    }
    /*
    uart_puts("SVC ");
    uart_puts_i(svc);
    uart_puts("\n");
    */
    unsigned long ret = 0;
    switch(svc)
    {
        case 0: // unknown
            uart_puts("SVC unknown exception\n");
            log_state();
        break;
        case 1: // exit
            do_exit();
        break;
        case 2: // exec
            do_exec(tf->regs[0], tf->regs[1]);
        break;
        case 3: // uart_write
            ret = do_uart_write(tf->regs[0], tf->regs[1]);
            tf->regs[0] = ret;
        return;
        case 4: // uart_read
            ret = do_uart_read(tf->regs[0], tf->regs[1]);
            tf->regs[0] = ret;
        return;
        case 5: // getpid
            ret = do_getpid();
            tf->regs[0] = ret;
        return;
        case 6: // fork
            do_fork(tf);
        return;
        case 7: // open
            ret = do_open(tf->regs[0], tf->regs[1]);
            tf->regs[0] = ret;
        return;
        case 8: // close
            ret = do_close(tf->regs[0]);
            tf->regs[0] = ret;
        return;
        case 9: // write
            ret = do_write(tf->regs[0], tf->regs[1], tf->regs[2]);
            tf->regs[0] = ret;
        return;
        case 10:// read
            ret = do_read(tf->regs[0], tf->regs[1], tf->regs[2]);
            tf->regs[0] = ret;
        return;
        default:
        break;
    }
}

void handle_irq()
{
    uart_puts("IRQ exception\n");
    log_state();

    timer_handler();
}

void handle_unknown_exception()
{
    uart_puts("Error: Unknown exception!\n");
    log_state();
}