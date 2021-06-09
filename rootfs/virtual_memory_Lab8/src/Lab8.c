#include "printf.h"
#include "sys.h"

int main(void) {
    printf("[Lab8.c] User program here!\n");
    
    int cnt = 0;
    if(call_sys_fork() == 0) {
        printf("[Lab8.c] Child here, pid: %d!\n", call_sys_gitPID());
        call_sys_fork();
        call_sys_fork();
        while(cnt < 10) {
            printf("pid: %d, sp: 0x%x cnt: %d\n", call_sys_gitPID(), &cnt, cnt++); // address should be the same, but the cnt should be increased indepndently
        }
        if (call_sys_gitPID() == 2) {
            printf("[Lab8.c] Call exec syscall...\n");
            char *fork_argv[] = {"Lab8_2.img", "87", 0};
            call_sys_exec("Lab8_2", fork_argv);
            printf("[Lab8.c] After exec, should not be printed\n");
        }
        call_sys_exit();
    } else {
        printf("[Lab8.c] Parent here, pid: %d!\n", call_sys_gitPID());
        int *a = (int *)(0x1000 * 16); // a non-mapped address.
        printf("%d\n", *a); // trigger simple page fault.
        printf("[Lab8.c] Should not be printed\n");
    }

    return 0;
}