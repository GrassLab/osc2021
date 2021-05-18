#include "lib.h"

int main(void) {
    printf("Fork Test, pid %d\n", get_pid());
    int cnt = 1;
    int ret = 0;
    if ((ret = fork()) == 0) { // child
        printf("pid: %d, cnt: %d, ptr: %p\n", get_pid(), cnt, &cnt);
        ++cnt;
        fork();
        while (cnt < 5) {
            printf("pid: %d, cnt: %d, ptr: %p\n", get_pid(), cnt, &cnt);
            sleep(1);
            ++cnt;
        }
    } else {
        printf("parent here, pid %d, child %d\n", get_pid(), ret);
    }
    return 0;
}
