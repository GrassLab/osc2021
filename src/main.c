#include "uart.h"
#include "shell.h"
#include "cpio.h"
#include "mm.h"
#include "sched.h"
#include "sys.h"

void foo(void) {
		thread_t *current = get_current();
		for (int i = 0; i < 5; i++) {
				print("Thread id: ");
				print_int(current->pid);
				print(", epoch: ");
				print_int(i);
				print("\n");
				delay(1000000);
				schedule();
		}
		thread_exit();
}

void user_test(void) {
		char *argv[] = {"argv_test", "-o", "arg2", 0, 0};
		do_exec("argv_test.img", argv, -1);
}

void user_test2(void) {
		char *argv[] = {"fork_test", "-o", "arg2", 0, 0};
		do_exec("fork_test.img", argv, -1);
}

static void kernel_init(void) {
		uart_init();
		read_cpio_archive();
		mm_init();
		sched_init();
}

int kernel_main(void) {
		kernel_init();

		/* require 1 */
		/*for (int i = 0; i < 5; i++)
				thread_create(&foo);
		idle();*/

		/* require 2 */
		thread_create(&user_test);
		//thread_create(&user_test2);
		idle();

		//run_shell();
		return 0;
}
