#include "printf.h"
#include "sys.h"
#include "uart.h"

int main(int argc, char **argv) 
{
    // init_printf(0, putc);

    // printf("\n[exec_argv_test]Argv Test, pid %d\n", call_sys_gitPID());
    // for (int i = 0; i < argc; ++i) {
    //     printf("%s\n", argv[i]);
    // }
    
    call_sys_write("help me !\n");
    call_sys_write("help me !\n");
    call_sys_write("help me !\n");
    call_sys_write("help me !\n");
    call_sys_write("help me !\n");
    //char *fork_argv[] = {"fork_test", 0};
    //call_sys_exec((unsigned long)&fork_test, fork_argv);
    call_sys_write("help me !\n");
    call_sys_exit();

    return 0;
}
