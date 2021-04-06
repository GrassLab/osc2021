#include "command.h"
#include "uart.h"
#include "string.h"
#include "reader.h"
#include "allocator.h"

void InputBufferOverflowMessage(char cmd[])
{
    uart_puts("Follow command: \"");
    uart_puts(cmd);
    uart_puts("\"... is too long to process.\n");

    uart_puts("The maximum length of input is 64.");
}

void CommandHelp()
{
    uart_puts("\n");
    uart_puts("Valid Command:\n");
    uart_puts("\thelp:\t\tprint this help.\n");
    uart_puts("\thello:\t\tprint \"Hello World!\"\n");
    uart_puts("\tls:\tls cpio file.\n");
    uart_puts("\tcat [filename]:\t cat cpio file content\n");
    uart_puts("\ttimestamp:\tget current timestamp.\n");
    uart_puts("\n");
}

void CommandHello()
{
    uart_puts("Hello World!\n");
}

void CommandCpiols()
{
    Cpiols();    
}

void CommandCpiocat(char arg[])
{
    Cpiocat(arg);
}

void CommandTimestamp()
{
    unsigned long int cnt_freq, cnt_tpct;
    char str[20];

    asm volatile(
        "mrs %0, cntfrq_el0 \n\t"
	"mrs %1, cntpct_el0 \n\t"
	: "=r" (cnt_freq), "=r" (cnt_tpct)
	:
    );

    ftoa((float)cnt_tpct / cnt_freq, str, 6);

    uart_send('[');
    uart_puts(str);
    uart_puts("]\n");
} 

void CommandBuddyInit()
{
    buddy_initialize();
}

void CommandBuddyLogList()
{
    buddy_log_list();    
}

void CommandBuddyLogTable()
{
    buddy_log_allocated_table();
}

void CommandBuddyLogPool()
{
    buddy_log_pool();
}

void CommandBuddyFree(const int section)
{
    buddy_free(section);
}

void CommandBuddyFreePool(int pool, int index)
{
    buddy_free_pool(pool, index);
}

void CommandBuddyAlloc(const int size)
{
    buddy_alloc(size);
}

void CommandNotFound(char *s)
{
    uart_puts("Err: command ");
    uart_puts(s);
    uart_puts(" not found, try <help>\n");
}

void CommandReboot()
{
    uart_puts("Start Rebooting...\n");

    *PM_RSTC = PM_PASSWORD | 0x20;
    *PM_WDOG = PM_PASSWORD | 100;

    while(1);
}
