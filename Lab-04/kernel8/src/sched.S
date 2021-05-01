#include "sched.h"

// This function is always called by _schedule in the context of a task that is already running.
// "next", the first argument can be a task that has never run, but it's the current task that
// ("prev") that kicks off "next". If "next" is new, it's pc is set to "ret_from_fork" and that's
// where it will return to. If "next" is not new, it means that it called "cpu_switch_to" from
// _schedule, and as a result, pc is set to return to _schedule.
// x0 = prev
// x1 = next
.globl cpu_switch_to
cpu_switch_to:
    // Store cpu_context from prev onto prev->cpu_context (NOT ON THE STACK!)
    mov x10, #THREAD_CPU_CONTEXT
    // x8 = prev + THREAD_CPU_CONTEXT (x8 = prev->cpu_context)
    add x8, x0, x10 
    mov x9, sp
    // The #16 here is Post-indexed meaning that, after this instruction, x8 = x8 + 16
    stp x19, x20, [x8], #16
    stp x21, x22, [x8], #16
    stp x23, x24, [x8], #16
    stp x25, x26, [x8], #16
    stp x27, x28, [x8], #16
    
    stp x29, x9, [x8], #16 // cpu_context.fp = x29, cpu_context.sp = x9
    // note that here, we're not really setting the current program counter but where
    // to return which could be "_schedule" or "ret_from_fork".
    str x30, [x8] // cpu_context.pc = x30 // (where to return)

    // Restore the cpu_context from next->cpu_context
    // I do this just in case x10 is used later. Worst case scenario, I have an extra instruction here.
    mov x10, #THREAD_CPU_CONTEXT
    // x8 = next + #THREAD_CPU_CONTEXT (x8 = next->cpu_context)
    add x8, x1, x10
    ldp x19, x20, [x8], #16
    ldp x21, x22, [x8], #16
    ldp x23, x24, [x8], #16
    ldp x25, x26, [x8], #16
    ldp x27, x28, [x8], #16
    ldp x29, x9, [x8], #16
    ldr x30, [x8]

    // This is the actual context switch (change stacks)
    mov sp, x9
    // ret now returns to where the new pc points to
    ret
   

