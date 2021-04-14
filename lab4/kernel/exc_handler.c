#include "printf.h"
#include "sched.h"
#include "mm.h"
#include "sysreg.h"

extern struct task_struct *__current_entry[];

void svc_handler(struct pt_regs *regs);

static void panic() {
    while(1) asm("wfe");
}

void sync_handler(struct pt_regs *regs) {
    unsigned long esr = read_sysreg(esr_el1);
    unsigned ec = ESR_ELx_EC(esr);
    unsigned iss = ESR_ELx_ISS(esr);

    switch (ec) {
    case ESR_ELx_EC_SVC64:
        /* iss[24-16] = res0  */
        /* iss[15-0]  = imm16 */
        if ((iss & 0xffff) == 0) {
            svc_handler(regs);
        }
        break;

    case ESR_ELx_EC_DABT_LOW:
        puts("Data abort exception");
        panic();

    case ESR_ELx_EC_IABT_LOW:
        puts("Instruction abort exception");
        panic();

    case ESR_ELx_EC_BRK_LOW:
        puts("Breakpoint exception");
        panic();

    default:
        printf("Unknown exception: EC=0x%x, ISS=0x%x\n\r", ec, iss);
        panic();
    }
}

void svc_handler(struct pt_regs *regs) {
    unsigned long spsr = read_sysreg(spsr_el1);
    unsigned long elr = read_sysreg(elr_el1);
    unsigned long esr = read_sysreg(esr_el1);

    printf("spsr_el1: 0x%lx, elr_el1: 0x%lx, esr_el1: 0x%lx\n\r", spsr, elr, esr);
}

void segv_handler() {
    struct task_struct *ts = __current_entry[0];
    kfree(ts->stack);
    kfree(ts->kstack);
    kfree(ts->user_prog);
    kfree(ts);
}