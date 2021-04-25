#include "uart.h"
#include "shell.h"
#include "printf.h"
#include "mm.h"
#include "timer.h"
#include "utils.h"
#include "entry.h"
#include "fork.h"
#include "sched.h"

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

void kernel_process(){
	printf("Kernel process started. EL %d\r\n", get_el());
  int err = 0;
	// int err = move_to_user_mode((unsigned long)&user_process);
	if (err < 0){
		printf("Error while moving process to user mode\n\r");
	} 
}

void process(char *array)
{
    while (1){
        for (int i = 0; i < 5; i++){
            uart_send(array[i]);
            delay(1000000);
        }
		schedule();
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
	
	// Requirement 1 - Implement the thread mechanism.
	// Test cases
    for(int i = 0; i < 2; ++i) { // N should
        int res = copy_process(PF_KTHREAD, (unsigned long)&foo, 0, 0);
		if (res < 0) {
			printf("error while starting kernel process");
			return 0;
		}
    }

	while (1) {
		printf("In kernel main()\n");
		kill_zombies(); // reclaim threads marked as DEAD
		schedule();
        delay(10000000);
    }


    
    // start shell
    // shell_start();

    return 0;
}
