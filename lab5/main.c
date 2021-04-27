#include "include/uart.h"
#include "include/shell.h"
#include "include/initrd.h"
#include "include/memAlloc.h"
#include "include/utils.h"
#include "include/task.h"
#include "include/switch.h"

void main()
{
    // set up serial console
    uart_init();
    //memory allocator init
    mem_init();
    //mem_status_dump();
    uart_puts("Hello World!\n");
    // echo everything back
    int el = get_el();
    uart_printf("Exception level: %d\n",el);
    task_struct* a = my_alloc(sizeof(task_struct));
    a->context.lr = 0;
    uart_printf("%x\n",&(a->context.x20));
    while(1) {
        shell();
    }
}
