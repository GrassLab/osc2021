#include "include/uart.h"
#include "include/shell.h"
#include "include/initrd.h"
#include "include/memAlloc.h"
#include "include/utils.h"
#include "include/task.h"
#include "include/switch.h"
#include "include/tmpfs.h"
#include "include/vfs.h"

void main()
{
    // set up serial console
    uart_init();
    //memory allocator init
    mem_init();
    //mem_status_dump();
    file_operations f_ops;
    tmpfsfopsGet(&f_ops);
    vfsInit(tmpfsSetup,f_ops.write,f_ops.read);

    uart_puts("Hello World!\n");
    // echo everything back
    int el = get_el();
    uart_printf("Exception level: %d\n",el);
    while(1) {
        shell();
    }
}
