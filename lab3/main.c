#include "uart.h"
#include "shell.h"
#include "allocator.h"
#include "dtb.h"

void main() {
    //uart_init();
    dtb_scan(uart_probe);
    init_memory();

    uart_put_str("welcome to rpi3\n");
    shell();
}