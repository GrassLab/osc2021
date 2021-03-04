#ifndef SHELL_H
#define SHELL_H

#define MAX_BUFFER_LEN 128

enum SPECIAL_CHARACTER
{
    BACK_SPACE = 8,
    LINE_FEED = 10,
    CARRIAGE_RETURN = 13,

    REGULAR_INPUT = 1000,
    NEW_LINE = 1001,

    UNKNOWN = -1,

};

void ShellStart();
void CommandController(enum SPECIAL_CHARACTER input, char c, char buffer[], int * counter);
enum SPECIAL_CHARACTER Parse(char c);

#endif
