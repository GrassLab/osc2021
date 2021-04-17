#include "uart.h"
#include "bootloader.h"
#include "dtb.h"

void main() {
    //uart_init();

    dtb_scan(uart_probe);

    bootloader();
}