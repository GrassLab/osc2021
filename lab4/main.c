#include "uart.h"
#include "shell.h"
#include "allocator.h"
#include "dtb.h"
#include "interrupt.h"

void main() {
    //uart_init();
    dtb_scan(uart_probe);
    init_memory();
    //enable_irq();
    core_timer_enable();

    uart_put_str("welcome to rpi3\n");
    shell();
}