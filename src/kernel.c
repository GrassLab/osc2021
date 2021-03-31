#include "mini_uart.h"
#include "shell.h"
#include "device_tree.h"
#include "allocator.h"

FrameArray *frame_array;

void kernel_start(void){
    uart_init();

    frame_array = NewFrameArray();
    // parse_dt();

    simple_shell();
}