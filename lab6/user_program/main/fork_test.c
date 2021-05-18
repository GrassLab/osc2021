#include "printf.h"
#include "sys_call.h"

int main(void) {
    printf("Fork Test, pid %d\r\n", getpid());
    int cnt = 1;
    int ret = 0;
    if ((ret = fork()) == 0) { // child
        printf("pid: %d, cnt: %d, ptr: %p\r\n", getpid(), cnt, &cnt);
        ++cnt;
        fork();
        while (cnt < 5) {
            printf("pid: %d, cnt: %d, ptr: %p\r\n", getpid(), cnt, &cnt);
            delay(1000000);
            ++cnt;
        }
    } else {
        printf("parent here, pid %d, child %d\r\n", getpid(), ret);
    }
}
