#include <printf.h>
#include <sched.h>
#include <mm.h>
#include <sysreg.h>
#include <syscall.h>
#include <uart.h>
#include <peripheral.h>
#include <timer.h>
#include <current.h>
#include <interrupt.h>
#include <preempt.h>

void svc_handler(struct pt_regs *regs);
void segv_handler();

void irq_handler(struct pt_regs *regs) {
    if (*CORE0_TIMER_IRQ_SRC & 2) {
        core_timer_handler();
        run_timers();
    }

    if (*AUXIRQ & 1) {
        // why does `*AUX_MU_IIR_REG & 1 == 0` not work ?
        uart_handler();
    }

    schedule();
}

void el1_sync_handler(struct pt_regs *regs) {
    panic("segfault ocurred in EL1");
}

void sync_handler(struct pt_regs *regs) {
    unsigned long esr = read_sysreg(esr_el1);
    unsigned ec = ESR_ELx_EC(esr);
    unsigned iss = ESR_ELx_ISS(esr);

    enable_interrupt();

    switch (ec) {
    case ESR_ELx_EC_SVC64:
        /* iss[24-16] = res0  */
        /* iss[15-0]  = imm16 */
        if ((iss & 0xffff) == 0) {
            svc_handler(regs);
        }
        break;

    case ESR_ELx_EC_DABT_LOW:
        /* Userland data abort exception */
        segv_handler();
        break;

    case ESR_ELx_EC_IABT_LOW:
        /* Userland instruction abort exception */
        segv_handler();
        break;

    case ESR_ELx_EC_BRK_LOW:
        panic("Breakpoint exception");

    default:
        panic("Unknown exception: EC=0x%x, ISS=0x%x", ec, iss);
    }

    schedule();
    disable_interrupt();
}

void svc_handler(struct pt_regs *regs) {
    unsigned nr = regs->regs[8];
    if (nr >= NR_syscalls) {
        panic("Unknown syscall number %d", regs->regs[8]);
    }

    /* exit syscall wont return */
    size_t ret = syscall_table[nr](regs);
    regs->regs[0] = ret;
}

/* TODO: signal */
void segv_handler() {
    printf("[Kernel] pid: %d segfault", current->pid);
    kill_task(current, -1);
}