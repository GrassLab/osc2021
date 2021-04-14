#include "shell.h"
#include "string.h"
#include "command.h"
#include "uart.h"
#include "printf.h"

void shell_start()
{
    int buffer_counter = 0;
    char input_char;
    char buffer[MAX_BUFFER_LEN];
    enum SPECIAL_CHARACTER input_parse;

    strset(buffer, 0, MAX_BUFFER_LEN);

    // new line head
    printf("# ");

    // read input
    while (1)
    {
        input_char = uart_getc();
        input_parse = parse(input_char);

        command_controller(input_parse, input_char, buffer, &buffer_counter);
    }
}

enum SPECIAL_CHARACTER parse(char c)
{
    if (!(c < 128 && c >= 0))
        return UNKNOWN;

    if (c == BACK_SPACE)
        return BACK_SPACE;
    else if (c == LINE_FEED || c == CARRIAGE_RETURN)
        return NEW_LINE;
    else
        return REGULAR_INPUT;
}

void command_controller(enum SPECIAL_CHARACTER input_parse, char c, char buffer[], int *counter)
{
    if (input_parse == UNKNOWN)
        return;

    if (input_parse == BACK_SPACE)
    {
        if ((*counter) > 0)
        {
            (*counter)--;
            printf("\b \b");
        }
    }
    else if (input_parse == NEW_LINE)
    {
        printf("%c", c);

        if ((*counter) == MAX_BUFFER_LEN)
        {
            input_buffer_overflow_message(buffer);
        }
        else
        {
            buffer[(*counter)] = '\0';

            if (!strcmp(buffer, "help"))
                command_help();
            else if (!strcmp(buffer, "hello"))
                command_hello();
            else if (!strcmp(buffer, "reboot"))
                command_reboot();
            else if (!strcmp(buffer, "cpio"))
                command_cpio();
            else if (!strcmp(buffer, "timer_on"))
                command_timer_on();
            else if (!strcmp(buffer, "timer_off"))
                command_timer_off();
            else if (!strcmp(buffer, "set_timeout"))
                command_set_timeout();
            else
                command_not_found(buffer);
        }

        (*counter) = 0;
        strset(buffer, 0, MAX_BUFFER_LEN);

        // new line head;
        printf("# ");
    }
    else if (input_parse == REGULAR_INPUT)
    {
        printf("%c", c);

        if (*counter < MAX_BUFFER_LEN)
        {
            buffer[*counter] = c;
            (*counter)++;
        }
    }
}