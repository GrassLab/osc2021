#include "../../lib/uart.h"
#include "../../lib/string.h"
#include "../../lib/sys_call.h"

int main(int argc, char **argv)
{
    char* temp_str = "User Program:\n";
    print(temp_str, strlen(temp_str));

    temp_str = "Argv Test, pid: ";
    print(temp_str, strlen(temp_str));
    itoa(getpid(), temp_str, 0);
    print(temp_str, strlen(temp_str));
    print("\n", 1);

    temp_str = "argc: ";
    print(temp_str, strlen(temp_str));
    itoa(argc, temp_str, 0);
    print(temp_str, strlen(temp_str));
    print("\n", 1);
    for (int i = 0; i < argc; ++i)
    {
        print(argv[i], strlen(argv[i]));
        print(" ", 1);
    }
    print("\n", 1);
    
    char * fork_argv[2];
    fork_argv[0] = "fork_test";
    fork_argv[1] = 0;
    exec("fork_test.img", fork_argv);
}