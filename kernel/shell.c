#include "shell.h"
#include "string.h"
#include "command.h"
#include "uart.h"

void ShellStart()
{
    int buffer_counter = 0;
    char input_char;
    char buffer[MAX_BUFFER_LEN];
    enum SPECIAL_CHARACTER input_parse;

    strset(buffer, 0, MAX_BUFFER_LEN);

    uart_puts("# ");

    while(1)
    {
        input_char = uart_getc();

	input_parse = Parse(input_char);

	CommandController(input_parse, input_char, buffer, &buffer_counter);
    }
}

void CommandController(enum SPECIAL_CHARACTER input_parse, char c, char buffer[], int * counter)
{
    if (input_parse == UNKNOWN)
	    return;

    if (input_parse == BACK_SPACE)
    {
        if ((*counter) > 0)
		(*counter)--;
	
	uart_send(c);
	uart_send(' ');
	uart_send(c);
    }
    else if (input_parse == NEW_LINE)
    {
        uart_send(c);

        if (*counter == MAX_BUFFER_LEN)
        {
            InputBufferOverflowMessage(buffer);
        }
        else
        {
            buffer[(*counter)] = '\0';

            if      (!strcmp(buffer, "help"     )) CommandHelp();
	    else if (!strcmp(buffer, "hello"    )) CommandHello();
	    else if (!strcmp(buffer, "timestamp")) CommandTimestamp();
	    else if (!strcmp(buffer, "reboot"   )) CommandReboot();
	    else                                   CommandNotFound(buffer);
	}

	(*counter) = 0;
	strset (buffer, 0, MAX_BUFFER_LEN);

	uart_puts("# ");
    }
    else if (input_parse == REGULAR_INPUT)
    {
        uart_send(c);

	if (*counter < MAX_BUFFER_LEN)
        {
	    buffer[(*counter)] = c;
	    (*counter)++;
	}
    }
}

enum SPECIAL_CHARACTER Parse(char c)
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
