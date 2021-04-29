#include "io.h"
#include "system_call.h"
#include "base_ops.h"

void main(int argc, char *argv[])
{
    int pid = sys_getpid();
    printf("Fork Test, pid %d\n", pid);
    int cnt = 1;
    int ret = sys_fork();

    if (ret == 0) { // child
        int pid = sys_getpid();
        printf("pid: %d, cnt: %d, ptr: %ld\n", pid, cnt, &cnt);
        ++cnt;
        sys_fork();
        while (cnt < 5) {
            int pid = sys_getpid();
            printf("pid: %d, cnt: %d, ptr: %ld\n", pid, cnt, &cnt);
            wait(100000000);
            
            ++cnt;
        }
    } else {
        int pid = sys_getpid();
        printf("parent here, pid %d, child %d\n", pid, ret);
    }
}