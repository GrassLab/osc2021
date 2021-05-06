#include "printf.h"
#include "sys.h"
#include "uart.h"

int main(void) {
    init_printf(0, putc);
    
    printf("\n[fork_test]Fork Test, pid %d\n", call_sys_gitPID());
    int cnt = 1;
    int ret = 0;
    printf("pid: %d, cnt: %d, cnt_adress: 0x%x\n", call_sys_gitPID(), cnt, &cnt);
    if ((ret = call_sys_fork()) == 0) { // child
        printf("pid: %d, cnt: %d, cnt_adress: 0x%x\n", call_sys_gitPID(), cnt, &cnt);
        ++cnt;
        call_sys_fork();
        while (cnt < 5) {
            printf("pid: %d, cnt: %d, cnt_adress: 0x%x\n", call_sys_gitPID(), cnt, &cnt);
            for(int i = 0;i < 100000;i++);
            ++cnt;
        }
    } else {
        printf("parent here, pid %d, child %d\n", call_sys_gitPID(), ret);
    }
    // dumpTasksState();
    printf("[exit] Task%d\n", call_sys_gitPID());
    call_sys_exit();
    return 0;
}