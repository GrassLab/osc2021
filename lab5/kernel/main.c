#include "printf.h"
#include "timer.h"
#include "sched.h"
#include "dtb.h"
#include "fs/cpio.h"
#include <interrupt.h>
#include <exec.h>
#include <asm/constant.h>
#include <sysreg.h>

// void delay(unsigned n) {
//     while(n--);
// }

// void foo() {
//     for(int i = 0; i < 10; ++i) {
//         printf("Thread id: %d %d\n", current->pid, i);
//         delay(100000);
//     }

//     /* since we set lr in cpu_context,
//      * after return the function will re execute itself again */
// }

// void main(void *_dtb_ptr) {
//     dtb_node *dtb = build_device_tree(_dtb_ptr);
//     init_cpio_storage(dtb);
//     set_init_thread();
//     for (int i = 0; i < 10; i++) {
//         schedule_kthread(&foo);
//     }
//     enable_core_timer();
//     enable_interrupt();
//     while (1);
// }

void run_init() {
    const char *argv[] = {"init", "-n", "0", NULL};
    kernel_exec_file("init", argv);

    panic("[Kernel] spawn init thread failed");
}

void enable_fpu() {
    asm(
        "mrs x0, cpacr_el1\n\t"
        "orr x0, x0, #0x300000\n\t"
        "msr cpacr_el1, x0\n\t"
    :::"x0");
}

void main(void *_dtb_ptr) {
    BUILD_BUG_ON(sizeof(struct pt_regs) != PT_REGS_SIZE);

    dtb_node *dtb = build_device_tree(_dtb_ptr);
    init_cpio_storage(dtb);
    set_init_thread();
    /* enable for EL0 */
    enable_fpu();

    schedule_kthread(&run_init);

    enable_core_timer();
    enable_interrupt();

    while (1);
}