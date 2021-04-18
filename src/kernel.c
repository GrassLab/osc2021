#include "mini_uart.h"
#include "shell.h"
#include "device_tree.h"
#include "allocator.h"
#include "str_tool.h"

FrameArray *frame_array;

void putc ( void* p, char c)
{
	uart_putc(c);
}

void kernel_start(void){
    uart_init();
    init_printf(0, putc);
    frame_array = NewFrameArray();
    // parse_dt();

    simple_shell();
}