#include "inc/syscall.h"

int main(int argc, char* argv[]){
    //asm volatile("svc 0  \n"::);
    //uart_write("hello in app1\n",14);
    uart_printf("Argv Test, pid %d\n",getpid());
    for(int i = 0; i <argc; ++i){
        uart_printf("%s\n",argv[i]);
    }
    char *fork_argv[] = {"fork_test", 0};
    exec("app2",fork_argv);
    cur_exit();
    return 0;
}
