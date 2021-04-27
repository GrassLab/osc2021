#include "lib/system_call.h"
int main(void) {
    uart_printf("Fork Test, pid %d\n", getpid());
    int cnt = 1;
    int ret = 0;
    if ((ret = fork()) == 0) { // child
        uart_printf("pid: %d, cnt: %d, ptr: %x\n", getpid(), cnt, &cnt);
        ++cnt;
        fork();
        while (cnt < 5) {
            uart_printf("pid: %d, cnt: %d, ptr: %x\n", getpid(), cnt, &cnt);
            delay(1000000);
            ++cnt;
        }
    } else {
        uart_printf("parent here, pid %d, child %d\n", getpid(), ret);
    }
    exit();
}