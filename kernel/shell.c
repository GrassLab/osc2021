#include "shell.h"
#include "command.h"
#include "../lib/uart.h"
#include "../lib/string.h"

void ShellStart()
{
    int buffer_counter = 0;
    char input_char;
    char buffer[MAX_BUFFER_LEN];
    enum SPECIAL_CHARACTER input_parse;

    strset(buffer, 0, MAX_BUFFER_LEN);

	CommandInit();

    uart_puts("# ");
	// thread test by default
	CommandThreadTest(1);

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
	
	uart_send((char)8); // BACK_CHAR
	uart_send(' ');
	uart_send((char)8);
    }
    else if (input_parse == NEW_LINE)
    {
        uart_puts("\n");
        
	if (*counter == MAX_BUFFER_LEN)
        {
            InputBufferOverflowMessage(buffer);
        }
        else
        {
            buffer[(*counter)] = '\0';	

	    int count = 0;
	    char command[50] = "", arg[50] = "";
            for (int i = 0; i < strlen(buffer) && i < 30 - 1; ++i)
	    {
	        if (buffer[i] == ' ' && count == 0)
		{
		    command[i] = '\0';
		    count = i + 1;
		}
		else if (count != 0)
		{
		    arg[i - count] = buffer[i];
		    arg[i - count + 1] = '\0';
		}
		else
		{
		    command[i] = buffer[i];
		    command[i + 1] = '\0';
		}
	    }

        if      (!strcmp(buffer,  "help"      )) CommandHelp();
	    else if (!strcmp(buffer,  "hello"     )) CommandHello();
	    else if (!strcmp(buffer,  "ls"        )) CommandCpiols();
	    else if (!strcmp(buffer,  "timestamp" )) CommandTimestamp();
	    else if (!strcmp(buffer,  "reboot"    )) CommandReboot();
		else if (!strcmp(buffer,  "loglist"   )) CommandBuddyLogList();
		else if (!strcmp(buffer,  "logpool"   )) CommandBuddyLogPool();
		else if (!strcmp(buffer,  "logtable"  )) CommandBuddyLogTable();
		else if (!strcmp(buffer,  "buddyinit" )) CommandBuddyInit();
		else if (!strcmp(command, "alloc"     )) CommandBuddyAlloc(atoi(arg));
		else if (!strcmp(command, "free"      )) CommandBuddyFree(atoi(arg));
		else if (!strcmp(command, "free16"    )) CommandBuddyFreePool(16,  atoi(arg));
		else if (!strcmp(command, "free32"    )) CommandBuddyFreePool(32,  atoi(arg));
		else if (!strcmp(command, "free64"    )) CommandBuddyFreePool(64,  atoi(arg));
		else if (!strcmp(command, "free128"   )) CommandBuddyFreePool(128, atoi(arg));
		else if (!strcmp(command, "threadtest")) CommandThreadTest(atoi(arg));
	    else if (!strcmp(command, "cat"       )) CommandCpiocat(arg);
		else if (!strcmp(command, "exe"       )) CommandCpioexe(arg);
	    else                                     CommandNotFound(buffer);
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
