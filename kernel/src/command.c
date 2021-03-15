#include "uart.h"
#include "string.h"
#include "cpio.h"

void input_buffer_overflow_message(char cmd[])
{
    uart_puts("Follow command: \"");
    uart_puts(cmd);
    uart_puts("\"... is too long to process.\n");

    uart_puts("The maximum length of input is 64.");
}

void command_help()
{
    uart_puts("\n");
    uart_puts("Valid Command:\n");
    uart_puts("\thelp:\t\tprint this help.\n");
    uart_puts("\thello:\t\tprint \"Hello World!\".\n");
    uart_puts("\ttimestamp:\tget current timestamp.\n");
    uart_puts("\treboot:\t\treboot Raspberry Pi.\n");
    uart_puts("\tcpio:\t\tread initramfs.cpio on the SD card.\n");
    uart_puts("\n");
}

void command_hello()
{
    uart_puts("Hello World!\n");
}

void command_timestamp()
{
    unsigned long int cnt_freq, cnt_tpct;
    char str[20];

    asm volatile(
        "mrs %0, cntfrq_el0 \n\t"
        "mrs %1, cntpct_el0 \n\t"
        : "=r"(cnt_freq), "=r"(cnt_tpct)
        :);

    ftoa(((float)cnt_tpct) / cnt_freq, str, 6);

    uart_send('[');
    uart_puts(str);
    uart_puts("]\n");
}

void command_cpio()
{
    uart_puts("\n");
    uart_puts("Valid Option:\n");
    uart_puts("\t1:\t\tGet the file list.\n");
    uart_puts("\t2:\t\tSearch for a specific file.\n");
    uart_puts("\n");
    uart_puts("# ");

    char choice = uart_getc();
    uart_send(choice);
    char new_line = uart_getc();
    uart_send('\n');

    switch (choice)
    {
    case '1':
        cpio_ls();
        break;

    case '2':
        {
            char file_name[100];
            char c;
            int counter = 0;

            uart_puts("# ");

            while (1)
            {
                c = uart_getc();
                // delete
                if ((c == 127) && counter > 0)
                {
                    counter--;
                    uart_puts("\b \b");
                }
                // new line
                else if ((c == 10) || (c == 13))
                {
                    file_name[counter] = '\0';
                    uart_send(c);
                    break;
                }
                // regular input
                else if (counter < 100)
                {
                    file_name[counter] = c;
                    counter++;
                    uart_send(c);
                }
            }

            cpio_find_file(file_name);
        }

        break;

    default:
        break;
    }
}

void command_not_found(char *s)
{
    uart_puts("Err: command ");
    uart_puts(s);
    uart_puts(" not found, try <help>\n");
}

void command_reboot()
{
    uart_puts("Start Rebooting...\n");

    *PM_WDOG = PM_PASSWORD | 100;
    *PM_RSTC = PM_PASSWORD | 0x20;

    // while(1);
}