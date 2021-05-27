#include "uart.h"
#include "shell.h"
#include "cpio.h"
#include "mm.h"
#include "sched.h"
#include "context.h"
#include "sys.h"
#include "vfs.h"
#include "utils.h"
#include "sdhost.h"

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
		do_exec("loop.img", argv, -1);
}

static void kernel_init(void) {
		uart_init();
		read_cpio_archive();
		mm_init();
		sched_init();
		vfs_init("tmpfs");
		sd_init();
}

int kernel_main(void) {
		kernel_init();

		/*
		char buf[512];
		int count = 0;
		for (int k =2080; k < 2082; k++) {
				readblock(k, buf);
				count = 0;
				for (int i = 0; i < 32; i++) {
						for (int j = 0;j < 8; j++) {
								print_hex(buf[16 * i + 2 * j]);
								print("(");
								print_int(count);
								print(")");
								print("|");
								count++;
								//print("|");
								print_hex(buf[16 * i + 2 * j + 1]);
								print("(");
								print_int(count);
								print(")");
								print("|");
								count++;
								print(" ");
						}
						print("\n");
				}
				print("\n");
				print_int(k);
				print("-> ==================================================\n");
	  }
		*/
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
