#include "syscall.h"

int main(void) {
    uart_printf("Fork Test, pid %d\n", getpid());
    int cnt = 1;
    int ret = 0;
    if ((ret = fork()) == 0) { // child
        uart_printf("child\n");
        uart_printf("pid: %d, cnt: %d, ptr: %x\n", getpid(), cnt, &cnt);
        ++cnt;
        fork();
        while (cnt < 5) {
            uart_printf("pid: %d, cnt: %d, ptr: %x\n", getpid(), cnt, &cnt);
            int i = 1000000;
            while(i--);
            ++cnt;
        }
    } else {
        uart_printf("parent here, pid %d, child %d\n", getpid(), ret);
    }
    exit();
    return 0;
}