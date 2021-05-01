#include <kernel.h>
#include <mmio.h>
#include <mini_uart.h>

uint32_t Kernel::totalThreads = 0;
ThreadStruct* Kernel::threadSPs = (ThreadStruct*)0x50000;

extern "C" {
    void (*ExceptionTable[])() = {
        Kernel::Sys_PrintExceptionReg,
        Kernel::Sys_TimerEnable,
        Kernel::Sys_TimerDisable,
        (void(*)())Kernel::Sys_CreateThread,
        Kernel::Sys_Schedule,
        Kernel::Sys_GetThreadID,
        Kernel::Sys_Exit
    };
}
uint64_t Kernel::Syscall(Sys callNum, uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4, uint64_t x5) {
    uint64_t ret;
    asm volatile(R"(
        mov x0, %x1
        mov x1, %x2
        mov x2, %x3
        mov x3, %x4
        mov x4, %x5
        mov x5, %x6
        svc #0
        mov %x0, x0
    )":"=r"(ret):
        "r"(callNum),
        "r"(x1),
        "r"(x2),
        "r"(x3),
        "r"(x4),
        "r"(x5)
    :"x0","x1","x2","x3","x4","x5");
    return ret;
}

void Kernel::Init() {
    totalThreads = 1;
}

void Kernel::Reset(uint32_t tick) {
    MMIO::set(MMIOREG::PM_RSTC, MMIO::PM_PASSWORD | 0x20);
    MMIO::set(MMIOREG::PM_WDOG, MMIO::PM_PASSWORD | tick);
}

void Kernel::Sys_PrintExceptionReg() {
    uint64_t spsr_el1, elr_el1, esr_el1;
    asm volatile(R"(
        mrs %x[spsr_el1], spsr_el1
        mrs %x[elr_el1], elr_el1
        mrs %x[esr_el1], esr_el1
    )":[spsr_el1]"=r"(spsr_el1), [elr_el1]"=r"(elr_el1), [esr_el1]"=r"(esr_el1));
    IO() << "spsr_el1: " << spsr_el1 << "\r\n";
    IO() << "elr_el1: " << elr_el1 << "\r\n";
    IO() << "esr_el1: " << esr_el1 << "\r\n";
    asm volatile("eret");
}

void Kernel::Sys_TimerHandler() {
    uint64_t cntpct_el0, cntfrq_el0;
    asm volatile(R"(
        mrs %x0, cntpct_el0
        mrs %x1, cntfrq_el0
    )":"=r"(cntpct_el0), "=r"(cntfrq_el0));
    IO() << "經過" << (cntpct_el0 / cntfrq_el0) << "秒\r\n";
    asm volatile(R"(
        mrs x0, cntfrq_el0
        msr cntp_tval_el0, x0
    )":::"x0");
}

void Kernel::Sys_TimerEnable() {
    asm volatile(R"(
        mov x0, 1
        msr cntp_ctl_el0, x0 // enable
        mrs x0, cntfrq_el0
        msr cntp_tval_el0, x0 // set expired time
        mov x0, 0
        msr spsr_el1, x0
    )":::"x0");
    MMIO::set(MMIOREG::CORE0_TIMER_IRQ_CTRL, 2);
    asm volatile("eret");
}

void Kernel::Sys_TimerDisable() {
    asm volatile(R"(
        mov x0, 0x3c0
        msr spsr_el1, x0
    )":::"x0");
    MMIO::set(MMIOREG::CORE0_TIMER_IRQ_CTRL, 0);
    asm volatile("eret");
}

void Kernel::Sys_Schedule() {
    uint64_t current, target;
    asm volatile(R"(
        mrs %x0, tpidr_el1
    )":"=r"(current));
    
    target = (current + 1 < totalThreads) ? current + 1 : 0;

    asm volatile(R"(
        stp x19, x20, [%x0, 16 * 0]
        stp x21, x22, [%x0, 16 * 1]
        stp x23, x24, [%x0, 16 * 2]
        stp x25, x26, [%x0, 16 * 3]
        stp x27, x28, [%x0, 16 * 4]
        stp fp, lr, [%x0, 16 * 5]
        mov x9, sp
        str x9, [%x0, 16 * 6]
        mrs x9, sp_el0
        str x9, [%x0, 8 * 13]
        mrs x9, elr_el1
        str x9, [%x0, 8 * 14]

        ldp x19, x20, [%x1, 16 * 0]
        ldp x21, x22, [%x1, 16 * 1]
        ldp x23, x24, [%x1, 16 * 2]
        ldp x25, x26, [%x1, 16 * 3]
        ldp x27, x28, [%x1, 16 * 4]
        ldp fp, lr, [%x1, 16 * 5]
        ldr x9, [%x1, 16 * 6]
        mov sp,  x9
        ldr x9, [%x1, 16 * 6 + 8]
        msr sp_el0, x9
        ldr x9, [%x1, 8 * 14]
        msr elr_el1, x9
        msr tpidr_el1, %x2
        eret
    )"::"r"(&Kernel::threadSPs[current]), "r"(&Kernel::threadSPs[target]), "r"(target));
}

void Kernel::Sys_CreateThread(uint64_t, void(*ptr)()) {
    uint64_t current, target;
    asm volatile(R"(
        mrs %x0, tpidr_el1
    )":"=r"(current));
    asm volatile(R"(
        stp x19, x20, [%x0, 16 * 0]
        stp x21, x22, [%x0, 16 * 1]
        stp x23, x24, [%x0, 16 * 2]
        stp x25, x26, [%x0, 16 * 3]
        stp x27, x28, [%x0, 16 * 4]
        stp fp, lr, [%x0, 16 * 5]
        mov x9, sp
        str x9, [%x0, 16 * 6]
        mrs x9, sp_el0
        str x9, [%x0, 16 * 6 + 8]
        mrs x9, elr_el1
        str x9, [%x0, 8 * 14]
    )"::"r"(&Kernel::threadSPs[current]));

    totalThreads++;

    asm volatile(R"(
        msr tpidr_el1, %x0
        msr elr_el1, %x1
        mov lr, %x2
        br %x3
    )"::"r"(totalThreads - 1), "r"(ptr), "r"(&Kernel::ExitThread), "r"(&Kernel::Sys_ReturnToEL0));
}

void Kernel::Sys_ReturnToEL0() {
    uint64_t tpidr_el1;
    asm volatile(R"(
        mrs %x0, tpidr_el1
    )":"=r"(tpidr_el1));
    uint64_t sp = 0x200000 + 0x10000 * tpidr_el1;
    asm volatile(R"(
        msr sp_el0, %x0
        mov %x0, 0x3c0
        msr spsr_el1, %x0
        eret
    )"::"r"(sp));
}

void Kernel::Sys_GetThreadID() {
    asm volatile(R"(
        mrs x0, tpidr_el1
        eret
    )");
}

void Kernel::Sys_Exit() {
    uint64_t tpidr_el1, target;
    asm volatile(R"(
        mrs %x[tpidr_el1], tpidr_el1
    )":[tpidr_el1]"=r"(tpidr_el1));
    totalThreads--;
    target = (tpidr_el1 + 1) % totalThreads;
    if (totalThreads != tpidr_el1 ) {
        threadSPs[tpidr_el1] = threadSPs[totalThreads];
    }
    asm volatile(R"(
        ldp x19, x20, [%x0, 16 * 0]
        ldp x21, x22, [%x0, 16 * 1]
        ldp x23, x24, [%x0, 16 * 2]
        ldp x25, x26, [%x0, 16 * 3]
        ldp x27, x28, [%x0, 16 * 4]
        ldp fp, lr, [%x0, 16 * 5]
        ldr x9, [%x0, 16 * 6]
        mov sp,  x9
        ldr x9, [%x0, 16 * 6 + 8]
        msr sp_el0, x9
        ldr x9, [%x0, 8 * 14]
        msr elr_el1, x9
        msr tpidr_el1, %x1
        eret
    )"::"r"(&threadSPs[0]), "r"(0));
}

void Kernel::ExitThread() {
    Syscall(Sys::EXIT);
}

extern "C"
void on_exception(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4, uint64_t x5) {
    IO() << "Exception\r\n";
}

extern "C"
void on_timer() {
    Kernel::Sys_TimerHandler();
}

extern "C"
void el1_to_el0() {
    Kernel::Sys_ReturnToEL0();
}
