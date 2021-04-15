#include "uart.h"
#include "string.h"
#include "shell.h"
#include "cpio.h"
#include "printf.h"
#include "utils.h"
#include "timer.h"

void input_buffer_overflow_message ( char cmd[] )
{
    uart_puts("Follow command: \"");
    uart_puts(cmd);
    uart_puts("\"... is too long to process.\n");

    uart_puts("The maximum length of input is 128.\n");
}

void command_help ()
{
    uart_puts("\n");
    uart_puts("Valid Command:\n");
    uart_puts("\thelp:\t\tprint all available commands.\n");
    uart_puts("\thello:\t\tprint \"Hello World!\".\n");
    uart_puts("\ttimestamp:\tget current timestamp.\n");
    uart_puts("\treboot:\t\treset rpi3.\n");
    uart_puts("\tls:\t\tPrint cpio file list.\n");
    uart_puts("\tcat {filename}:\tPrint content in {filename} \n");
    uart_puts("\tma:\t\tSystem of memory allcator \n");
    uart_puts("\tcurrentEL:\tPiint current exception level(You can't use it in EL0)\n");
    uart_puts("\tcpio_svc:\tJump to user program in cpio archive \n");
    uart_puts("\tcoreTimerOn:\tEnable core0 timer interrupt (Interrupt periodically) \n");
    uart_puts("\tcoreTimerOff:\tDisable core0 timer interrupt \n");
    uart_puts("\tsetTimeout [MESSAGE] [SECONDS]:\tprints [MESSAGE] after [SECONDS] ([SECONDS] need lower than 45)\n");
    uart_puts("\n");
}

void command_hello ()
{
    uart_puts("Hello World!\n");
}

void command_timestamp ()
{
    unsigned long int cnt_freq, cnt_tpct;
    char str[20];

    asm volatile(
        "mrs %0, cntfrq_el0 \n\t"
        "mrs %1, cntpct_el0 \n\t"
        : "=r" (cnt_freq),  "=r" (cnt_tpct)
        :
    );

    ftoa( ((float)cnt_tpct) / cnt_freq, str, 6);

    uart_send('[');
    uart_puts(str);
    uart_puts("]\n");
}

void command_not_found (char * s) 
{
    uart_puts("Err: command ");
    uart_puts(s);
    uart_puts(" not found, try <help>\n");
}

void command_reboot ()
{
    uart_puts("Start Rebooting...\n");

    *PM_WDOG = PM_PASSWORD | 0x20;
    *PM_RSTC = PM_PASSWORD | 100;
    
	while(1);
}

void command_cpio_ls(void *initramfs_addr) {
    //printf("command_cpio_ls() execuated");
    //printf("%p", initramfs_addr);
    cpio_ls(initramfs_addr);
}

void command_getCpioFile(void *initramfs_addr, char *buf)
{
    unsigned long fileSize;
    char *result = cpio_get_file(initramfs_addr, buf, &fileSize);
    if (result != NULL) {
        for (int i = 0;i < fileSize;i++) {
            printf("%c", result[i]);
        }
        printf("\n");
    } else {
        printf("'%s' file  not exist!\n", buf);
    }
}

void command_cpio_svc() 
{
    unsigned long fileSize;
    char *result = cpio_get_file((void *) INITRAMFS_ADDR, "test_svc.elf", &fileSize);

    char *program_address = result + 120;
    printf("cpio starting addres0x{%x}\n", result);
    printf("program starting address: 0x{%x}\n", program_address);


    // Jump to user program in initramfs.cpio. And before jump to user program we should do following thing
    // 1. set EL0 stack pointer before jump into user program(user mode)
    // 2. set spsr_el1 to enable irq interrupt and use user stack
    // 3. set elr_el1 with starting address of user program
    // Note: 
    // 1. In ARM, mov instrcution  can only encode an immediate constant that can be 
    // represented as an 8-bit immediate value, shifted by any even power of two.
    // 2. we hardcode starting address of user porgram, maybe there are other better metholds
    asm volatile(
        "mov x0, 0x340        \n \
         msr spsr_el1, x0     \n \
         mov x0, 0x8000000    \n \
         add x0, x0, 0x28c    \n \
         msr elr_el1, x0      \n \
         ldr x1, =0x40000     \n \
         msr sp_el0, x1       \n \
         eret");

    printf("Error!, End cpio_svc \n"); // For failsafe, should not execute it
}


void command_current_el()
{
    int el = get_el();
    printf("Current Exception Level: %d \r\n", el);
}

void commnad_coreTimerOn()
{
    asm volatile("svc #2");
}

void commnad_coreTimerOff()
{
    asm volatile("svc #3");
}

void coomand_setTimeout(char *buf)
{
    uint64_t duration = 0;
    for (int i = 0;buf[i];i++) {
        if (buf[i] == ' ') {
            for (int j = i + 1;buf[j];j++) {
                duration = duration * 10 + (buf[j] - '0');
            }
            buf[i] = '\0'; // split message and sconds
            break;
        }
    }
    // printf("parse msg: %s\n", buf);
    // printf("parse seconds: %d\n", duration);
    add_timer(print_timeoutEventInfo, buf, duration);   
}

