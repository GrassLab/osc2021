#include "uart.h"
#include "shell.h"
#include "cpio.h"
#include "mm.h"

int main() {
		//uart_init();
		read_cpio_archive();
		mm_init();
		run_shell();
		return 0;
}
