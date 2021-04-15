#include "uart.h"
#include "shell.h"
#include "dtb.h"

void main() {
    //uart_init();
    dtb_scan(uart_probe);
    
    uart_put_str("welcome to rpi3\n");
    shell();
}