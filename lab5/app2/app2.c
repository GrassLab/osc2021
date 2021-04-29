#include "inc/syscall.h"

int main(int argc, char* argv[]){
    uart_printf("Fork Test, pid:%d\n",getpid());
    int ret = 0;
    int cnt = 1;

    //uart_printf("addr of len:%x\n", &len);
    if((ret=sys_fork()) == 0){
        uart_printf("pid:%d, cnt:%d, ptr:%x\n",getpid(), cnt, &cnt);
        ++cnt;
        sys_fork();
        while(cnt < 5){
            uart_printf("pid:%d, cnt:%d, ptr:%x\n",getpid(), cnt, &cnt);
            ++cnt;
        }
    }else{
        uart_printf("parent here, pid: %d\n",getpid());
    }
    cur_exit();
    return 0;
}
