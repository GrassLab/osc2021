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
    unsigned long elr = read_sysreg(elr_el1);
    unsigned long esr = read_sysreg(esr_el1);
    unsigned ec = ESR_ELx_EC(esr);
    unsigned iss = ESR_ELx_ISS(esr);
    panic("segfault ocurred in EL1: PC=0x%lu, EC=%x, ISS=0x%x", elr, ec, iss);
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
        segv_handler(regs);
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
void segv_handler(struct pt_regs *regs) {
    printf("[Segfault] pid: %d, pc=0x%016lx\r\n", current->pid, read_sysreg(elr_el1));
    for (int i = 0; i < 30; i += 2)
        printf("x%d=0x%016lx x%d=0x%016lx\r\n", i, regs->regs[i], i+1, regs->regs[i+1]);
    kill_task(current, -1);
}