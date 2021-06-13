#include "printf.h"
#include "sys_call.h"

int main(void) {
    printf("Preempt Test, pid %d\r\n", getpid());
    fork();
    for (int i = 0; i < 2000000; ++i) {
        printf("pid: %d, i: %d, ptr: %p\r\n", getpid(), i, &i);
    }
}
