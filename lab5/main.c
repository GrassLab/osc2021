#include "include/uart.h"
#include "include/shell.h"
#include "include/allocator.h"
#include "include/dtb.h"
#include "include/interrupt.h"
#include "include/scheduler.h"
#include "include/systemcall.h"
#include "include/process.h"

void user_test() {
    char* argv[4];
    argv[0] = "argv_test";
    argv[1] = "-o";
    argv[2] = "arg2";
    argv[3] = 0;
    exec("argv_test", argv);
}

void idle() {
    while(1) {
        kill_zombies(); // reclaim threads marked as DEAD
        schedule(); // switch to any other runnable thread
    }
}

void kernel_main() {
    dtb_scan(uart_probe);
    init_memory();
    core_timer_enable();
    
    init_thread();
    uart_put_str("welcome to rpi3\n");
    
    thread_create(user_test);
    thread_create(user_test);
    idle();
}