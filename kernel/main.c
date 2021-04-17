#include "uart.h"
#include "shell.h"
#include "buddy.h"
#include "slab.h"

void main() {
    uart_init();
    buddy_init();
    slab_init();

    shell_welcome_message();

    while(1) {
        shell_start();
    }
}