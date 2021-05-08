#include "printf.h"
#include "timer.h"
#include "sched.h"
#include "dtb.h"
#include "fs/cpio.h"
#include <interrupt.h>

void delay(unsigned n) {
    while(n--);
}

void foo(){
    for(int i = 0; i < 10; ++i) {
        printf("Thread id: %d %d\n", current->pid, i);
        delay(10000000);
        schedule();
    }
}

int main(void *_dtb_ptr) {
    dtb_node *dtb = build_device_tree(_dtb_ptr);
    init_cpio_storage(dtb);
    set_init_thread();
    for (int i = 0; i < 10; i++) {
        schedule_kthread(&foo);
    }
    enable_core_timer();

    //shell();

    enable_interrupt();
    while (1);
    return 0;
}
