#ifndef SHELL_H
#define SHELL_H

#define MAX_BUFFER_LEN 256

#define KERNEL_SHELL_DISABLE 0
#define KERNEL_SHELL_ENABLE  1
extern int kernel_shell_status;

enum SPECIAL_CHARACTER
{
    BACK_SPACE = '\177',
    LINE_FEED = 10,
    CARRIAGE_RETURN = 13,
    
    
    REGULAR_INPUT = 1000,
    NEW_LINE = 1001,
    
    UNKNOWN = -1,

};

void shell_start ();
char read_transmit_asynchronous_procoessing(char buffer[], int * buffer_counter);
enum SPECIAL_CHARACTER parse ( char );
void command_controller ( enum SPECIAL_CHARACTER, char c, char [], int *);

#endif