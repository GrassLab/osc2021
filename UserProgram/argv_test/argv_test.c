#include "../../lib/uart.h"
#include "../../lib/string.h"
#include "../../lib/sys_call.h"

int main(int argc, char **argv)
{
    char* temp_str = "User Program:\n";
    write(temp_str, strlen(temp_str));

    temp_str = "Argv Test, pid: ";
    write(temp_str, strlen(temp_str));
    itoa(getpid(), temp_str, 0);
    write(temp_str, strlen(temp_str));
    write("\n", 1);

    temp_str = "argc: ";
    write(temp_str, strlen(temp_str));
    itoa(argc, temp_str, 0);
    write(temp_str, strlen(temp_str));
    write("\n", 1);
    for (int i = 0; i < argc; ++i)
    {
        write(argv[i], strlen(argv[i]));
        write(" ", 1);
    }
    write("\n", 1);
    
    char * fork_argv[2];
    fork_argv[0] = "fork_test";
    fork_argv[1] = 0;
    exec("fork_test.img", fork_argv);
}