#include "inc/syscall.h"

int main(void) {
	char buffer[500];
	uart_printf("read test: \n");
	int len=uart_read(buffer,500);
	uart_printf("%s,len: %d\n",buffer,len);
    len = uart_write(buffer,2);

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
        uart_printf("parent here, pid %d, child %d, ptr: %x\n", getpid(), ret, &cnt);
    }
    exit();
}