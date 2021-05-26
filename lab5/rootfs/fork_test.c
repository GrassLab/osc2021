#include "utils.h"
#include "systemcall.h"

int main(void) {
    char str[15], ptr_str[10];

    int pid = getpid();
    uart_write("Fork Test, pid ", 15);
    int2str(pid, str, 15);
    uart_write(str, int_len(pid));
    uart_write("\n", 1);

    int cnt = 1;
    int ret = 0;
    if ((ret = fork()) == 0) { // child
        pid = getpid();
        uart_write("pid: ", 5);
        int2str(pid, str, 15);
        uart_write(str, int_len(pid));
        uart_write(", cnt: ", 7);
        int2str(cnt, str, 15);
        uart_write(str, int_len(cnt));
        uart_write(", ptr: ", 7);
        ptr2str((unsigned long)&cnt, ptr_str);
        uart_write(ptr_str, 10);
        uart_write("\n", 1);
        
        ++cnt;
        fork();
        while (cnt < 5) {
            pid = getpid();
            uart_write("pid: ", 5);
            int2str(pid, str, 15);
            uart_write(str, int_len(pid));
            uart_write(", cnt: ", 7);
            int2str(cnt, str, 15);
            uart_write(str, int_len(cnt));
            uart_write(", ptr: ", 7);
            ptr2str((unsigned long)&cnt, ptr_str);
            uart_write(ptr_str, 10);
            uart_write("\n", 1);
            delay(10);
            ++cnt;
        }
    } else {
        pid = getpid();
        uart_write("parent here, pid ", 17);
        int2str(pid, str, 15);
        uart_write(str, int_len(pid));
        uart_write(", child ", 8);
        int2str(ret, str, 15);
        uart_write(str, int_len(ret));
        uart_write("\n", 1);
    }
    return 0;
}