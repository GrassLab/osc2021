#include "shell.h"
#include "string.h"
#include "command.h"
#include "uart.h"
#include "cpio.h"
#include "printf.h"
#include "mm.h"
#include "exception.h"

extern int uart_read_idx;
extern char UART_READ_BUFFER[MAX_BUFFER_LEN];
extern int uart_transmit_idx;
extern char UART_TRANSMIT_BUFFER[MAX_BUFFER_LEN];
void shell_start () 
{
    int buffer_counter = 0;
    char input_char;
    char buffer[MAX_BUFFER_LEN];
    enum SPECIAL_CHARACTER input_parse;

    strset (buffer, 0, MAX_BUFFER_LEN);   
    // enable irq 
    enable_irq_persist();

    // enable mini uart read/write asynchronous interrupt 
    enable_uart_interrupt();

    // new line head
    uart_puts("# ");

    // read input
    while(1)
    {
        input_char = read_transmit_asynchronous_procoessing(buffer, &buffer_counter);
        
        input_parse = parse ( input_char );

        command_controller ( input_parse, input_char, buffer, &buffer_counter);
    }
}

char read_transmit_asynchronous_procoessing(char buffer[], int * buffer_counter)
{
    while (1)
    {
        // There are not processing chars in read buffer
        if (uart_read_idx > 0)
        {
            /** 
             * Read one char at the front of read buffer
             * Should it be a crtical section? 
             * Because the variables in this critical section may be modified in uart IRQ hanlder at the same time
             
             */
            //printf("buffer_counter = %d, uart_read_idx = %d\n", *buffer_counter, uart_read_idx);
            disable_irq(); // critical section
            char input_char = UART_READ_BUFFER[uart_read_idx - 1];
            uart_read_idx--;
            enable_irq(); // End critical section

            /** 
             * Write to the tail end of transmit buffer
             * Also, it should be ciritical section. Because it's shared variables in shell.c and uart.c
             * 
             */
            disable_irq(); // critical section 
            UART_TRANSMIT_BUFFER[uart_transmit_idx] = input_char;
            uart_transmit_idx++;
            enable_irq(); // End critical section

            enable_uart_transmit_interrupt(); // enable transmit interrupt to print on screen
            
            return input_char;
        }

        // ToDo : 
        // Busy Waiting for printing all chars remaining in transmit buffer to avoid concurrency(race condition) problem. 
        // Because it's possible when doing printf() function then transmit exception occur causing 
        // race condiion?? 
        // (It's race condition possible happen? Maybe not. But processor is too strong, so it's difficult to check that)
        // For safe problem in future, we still add busy waitng code 
        while (uart_transmit_idx != 0);
    }
}

enum SPECIAL_CHARACTER parse ( char c )
{
    if (c == BACK_SPACE) // back space '\177'
        return BACK_SPACE;
        
    if ( !(c < 128 && c >= 0) )
        return UNKNOWN;

    if ( c == LINE_FEED || c == CARRIAGE_RETURN )
        return NEW_LINE;
    else
        return REGULAR_INPUT;    
}

void command_controller ( enum SPECIAL_CHARACTER input_parse, char c, char buffer[], int * counter )
{
    if ( input_parse == UNKNOWN )
        return;
    
    // Special key
    if ( input_parse == BACK_SPACE )
    {
        if (  (*counter) > 0 ) {
            (*counter) --;
            uart_puts("\b \b");
        }
    }
    else if ( input_parse == NEW_LINE )
    {
        //uart_send(c);
        
        if ( (*counter) == MAX_BUFFER_LEN ) 
        {
            input_buffer_overflow_message(buffer);
        }
        else 
        {
            buffer[(*counter)] = '\0';

            if      ( !strcmp(buffer, "help"        ) ) command_help();
            else if ( !strcmp(buffer, "hello"       ) ) command_hello();
            else if ( !strcmp(buffer, "timestamp"   ) ) command_timestamp();
            else if ( !strcmp(buffer, "reboot"      ) ) command_reboot();
            else if ( !strcmp(buffer, "ls"          ) ) command_cpio_ls((void *) INITRAMFS_ADDR);
            else if ( !strncmp(buffer, "cat ", 3    ) ) command_getCpioFile((void *) INITRAMFS_ADDR, buffer + 4);
            else if ( !strcmp(buffer, "ma"          ) ) mm_init();
            else if ( !strcmp(buffer, "cpio_svc"    ) ) command_cpio_svc(); // eret jump to cpio assemlby file(user program), and svc return to kernel
            else if ( !strcmp(buffer, "currentEL"   ) ) command_current_el();
            else if ( !strcmp(buffer, "coreTimerOn" ) ) commnad_coreTimerOn();
            else if ( !strcmp(buffer, "coreTimerOff") ) commnad_coreTimerOff();
            else if ( !strncmp(buffer, "setTimeout ", 10)) coomand_setTimeout(buffer + 11);
            else                                        command_not_found(buffer);
        }

        // reset Command buffer
        (*counter) = 0;
        strset (buffer, 0, MAX_BUFFER_LEN); 

        // new line head;
        uart_puts("# ");
    }
    else if ( input_parse == REGULAR_INPUT )
    {
        //uart_send(c);
        if ( *counter < MAX_BUFFER_LEN)
        {
            buffer[*counter] = c;
            (*counter) ++;
        }
    }
}