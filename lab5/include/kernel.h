#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

class ThreadStruct {
    uint64_t registers[15];
};

enum class Sys {
    PRINT_EXECPTION_REG,
    TIMER_ENABLE,
    TIMER_DISABLE,
    CREATE_THREAD,
    SCHEDULE,
    GET_THREAD_ID,
    EXIT
};

class Kernel {
public:
    static void Init();
    static void Reset(uint32_t tick);
    inline static void Delay(uint64_t cycle) {
        while (cycle--) asm volatile("nop");
    }
    static void Sys_PrintExceptionReg();
    static void Sys_TimerEnable();
    static void Sys_TimerDisable();
    static void Sys_Schedule();
    static void Sys_CreateThread(uint64_t, void(*ptr)());
    static void Sys_ReturnToEL0();
    static void Sys_TimerHandler();
    static void Sys_GetThreadID();
    static void Sys_Exit();
    static void ExitThread();
    static uint64_t Syscall(Sys syscallNum, uint64_t x1 = 0, uint64_t x2 = 0, uint64_t x3 = 0, uint64_t x4 = 0, uint64_t x5 = 0);
    static uint32_t totalThreads;
    static ThreadStruct * threadSPs;
};
#endif
