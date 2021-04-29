#include "inc/syscall.h"

int main(int argc, char* argv[]){
    //int len = uart_printf("argc: %d\n",argc);
    //int len = uart_write("test\n",5);
    //uart_printf("len %d\n",len);
    //char buf[128];
    //uart_printf("%s\n","Please enter read context");
    //uart_read(buf,128);
    //uart_printf("Your context:%s\n",buf);
    //int pid = getpid();
    //uart_printf("Your pid:%d\n",pid);
    //dumpState();
    //for(int i =0 ;i<argc ; ++i){
    //    uart_printf("argv: %s\n",argv[i]);
    //}
    //char *fork_argv[] = {"fork_test", 0 };
    //exec("app2", fork_argv);
    //cur_exit();
        
    uart_printf("Argv Test, pid %d\n",getpid());
    for(int i = 0; i <argc; ++i){
        uart_printf("%s\n",argv[i]);
    }
    char *fork_argv[] = {"fork_test", 0};
    exec("app2",fork_argv);
    cur_exit();
    return 0;
}
