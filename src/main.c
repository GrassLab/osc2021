#include "uart.h"
#include "shell.h"
#include "printf.h"
#include "mm.h"
#include "timer.h"
#include "utils.h"
#include "entry.h"
#include "fork.h"
#include "sched.h"
#include "sys.h"
#include "cpio.h"
/* Initial Logo */
//   ___  ____  ____ ___   ____   ___ ____  _  __   __                 
//  / _ \/ ___||  _ \_ _| |___ \ / _ \___ \/ | \ \ / /   _ _ __   __ _ 
// | | | \___ \| | | | |    __) | | | |__) | |  \ V / | | | '_ \ / _` |
// | |_| |___) | |_| | |   / __/| |_| / __/| |   | || |_| | | | | (_| |
//  \___/|____/|____/___| |_____|\___/_____|_|   |_| \__,_|_| |_|\__, |
//                                                               |___/ 
char * init_logo = "\n\
  ___  ____  ____ ___   ____   ___ ____  _  __   __\n\
 / _ \\/ ___||  _ \\_ _| |___ \\ / _ \\___ \\/ | \\ \\ / /   _ _ __   __ _\n\
| | | \\___ \\| | | | |    __) | | | |__) | |  \\ V / | | | '_ \\ / _` |\n\
| |_| |___) | |_| | |   / __/| |_| / __/| |   | || |_| | | | | (_| |\n\
 \\___/|____/|____/___| |_____|\\___/_____|_|   |_| \\__,_|_| |_|\\__, |\n\
                                                              |___/\n\n";


void foo(){
    for(int i = 0; i < 10; ++i) {
        printf("Thread id: %d %d\n", current->pid, i);
        delay(1000000);
        schedule();
    }
    
    exit_process();
}

void fork_test()
{
    /* Test fork */
    printf("\n[fork_test]Fork Test, pid %d\n", call_sys_gitPID());
    int cnt = 1;
    int ret = 0;
    printf("pid: %d, cnt: %d, cnt_adress: 0x%x\n", call_sys_gitPID(), cnt, &cnt);
    if (call_sys_fork() == 0) { // child
        printf("pid: %d, cnt: %d, cnt_adress: 0x%x\n", call_sys_gitPID(), cnt, &cnt);
        ++cnt;
        call_sys_fork();
        while (cnt < 5) {
            printf("pid: %d, cnt: %d, cnt_adress: 0x%x\n", call_sys_gitPID(), cnt, &cnt);
            delay(1000000);
            ++cnt;
        }
    } else {
        printf("parent here, pid %d, child %d\n", call_sys_gitPID(), ret);
    }
    // dumpTasksState();
    printf("[exit] Task%d\n", call_sys_gitPID());
    call_sys_exit();
}

int exec_argv_test(int argc, char **argv) {
    printf("\n[exec_argv_test]Argv Test, pid %d\n", call_sys_gitPID());
    for (int i = 0; i < argc; ++i) {
        printf("%s\n", argv[i]);
    }
    
    char *fork_argv[] = {"fork_test", 0};
    call_sys_exec("fork_test", fork_argv);
    return -1;
}

void user_process(){
    /* Test syscall write */
    // call_sys_write("[user_process]User process started\n");
    
    // /* Test syscall getPID */
    // int current_pid = call_sys_gitPID();
    // printf("[getPID] Test syscall getpid\n");
    // printf("[getPID]Current Pid = %d\n", current_pid);

    // /* Test syscall uart_write*/
    // printf("[uart_write] Test syscall uart_write\n");
    // int size = call_sys_uart_write("[uart_write] syscall test\n", 26);
    // printf("[uart_write] How many byte written = %d\n", size);

    // /* Test syscall uart_read */
    // printf("[uart_read] Test syscall uart_read\n");
    // char uart_read_buf[20];
    // printf("[uart_read] Enter your input: ");
    // size = call_sys_uart_read(uart_read_buf, 4);
    // printf("\n[uart_read] Read buf = %s, How many byte read = %d\n", uart_read_buf, size);

    // /* Test syscall malloc */
    // printf("[malloc] Test syscall malloc\n");
    // void *malloc_return = call_sys_malloc(PAGE_SIZE);
    // printf("[malloc] Allocated starting address of page = 0x%x\n", malloc_return);

    /* Test syscall exec with argument passing */
    char* argv[] = {"argv_test", "-o", "arg2", 0};
    call_sys_exec("argv_test", argv);

    /* syscall exit */
    printf("[exit] Task%d exit\n", call_sys_gitPID());
    call_sys_exit();
}


void kernel_process(){
    printf("[kernel_process]Kernel process started. EL %d\r\n", get_el());
    int err = move_to_user_mode((unsigned long)&user_process);
    if (err < 0){
        printf("Error while moving process to user mode\n\r");
    } 
}

int main()
{
    // set up serial console
    uart_init();
    
    // Initialize printf
    init_printf(0, putc);

    // Initialize memory allcoator
    mm_init();

    // Initialize timer list for timeout events
    timer_list_init();

    // say hello
    printf(init_logo);

    // enable IRQ interrupt
    enable_irq();

    /* Test cases */
    // Requirement 1 - Implement the thread mechanism. 
    // for(int i = 0; i < 2; ++i) { // N should
    //     int res = copy_process(PF_KTHREAD, (unsigned long)&foo, 0, 0);
    //     if (res < 0) {
    //      printf("error while starting kernel process");
    //      return 0;
    //    }
    // }

    // Requirement 2 
    // int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0, 0);
    // if (res < 0) {
    //     printf("error while starting kernel process");
    //     return 0;
    // }
    

    // while (1) {
    //     printf("In kernel main()\n");
    //     dumpTasksState();
    //     kill_zombies(); // reclaim threads marked as DEAD
    //     schedule();
    //     delay(10000000);
    // }


    
    // start shell
    shell_start();

    return 0;
}

void process(char *array)
{
    while (1){
        for (int i = 0; i < 5; i++){
            uart_send(array[i]);
            delay(1000000000);
        }
        schedule();
    }
}