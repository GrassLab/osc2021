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
#include "fat.h"

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
		char *argv[] = {"argv_test", 0};
		do_exec("argv_test.img", argv, -1);
		thread_exit();
}

void user_test2(void) {
		char *argv[] = {"fork_test", "-o", 0};
		do_exec("loop.img", argv, -1);
		thread_exit();
}

static void kernel_init(void) {
		uart_init();
		read_cpio_archive();
		mm_init();
		sched_init();
		sd_init();
		vfs_init("tmpfs");
}

int kernel_main(void) {
		kernel_init();

		/*mount_t m;
		fat_set_mount(&m, "sd");
		vnode_t *r = m.root;
		list_head_t *pos = r->node.next;
		while (pos != &r->node) {
				vnode_t *t = list_entry(pos, vnode_t, node);
				print(t->name);
				print("\n");
				pos = pos->next;
		}*/

		/* require 1 */
		/*for (int i = 0; i < 5; i++)
				thread_create(&foo);
		idle();*/

		/* require 2 */
		thread_create(&user_test);
		thread_create(&user_test2);
		idle();
		//run_shell();
		return 0;
}
