#include "uart.h"
#include "shell.h"
#include "cpio.h"
#include "mm.h"
#include "exception.h"
#define EXEC_ADDR      0x1000000

int main() {
		uart_init();
		read_cpio_archive();
		/*int i = 1;
		char *exec_addr = (char*)EXEC_ADDR;
		for (int j = 0; j < file_list[i].file_size; j++)
				*(exec_addr + j) = *(file_list[i].file_content + j);
		from_el1_to_el0(EXEC_ADDR);*/
		mm_init();
		run_shell();
		return 0;
}
