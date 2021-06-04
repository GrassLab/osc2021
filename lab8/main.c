#include "include/uart.h"
#include "include/shell.h"
#include "include/initrd.h"
#include "include/memAlloc.h"
#include "include/utils.h"
#include "include/task.h"
#include "include/switch.h"
#include "include/tmpfs.h"
#include "include/vfs.h"
#include "include/sd.h"
#include "include/fat32.h"

void main()
{
    // set up serial console
    uart_init();
    //memory allocator init
    mem_init();
    sd_init();
    //file_operations f_ops;
    //tmpfsfopsGet(&f_ops);
    vfsInit(fatSetup);
/*test sd init
    unsigned char* buf = (unsigned char*)my_alloc(512);
    readblock(0,buf);
    if(buf[510]== 0x55){
        uart_puts("success\n");
    }else{
        uart_puts("nnono\n");
    }
    */
    uart_puts("Hello World!\n");
    // echo everything back
    int el = get_el();
    uart_printf("Exception level: %d\n",el);
    while(1) {
        shell();
    }
}
