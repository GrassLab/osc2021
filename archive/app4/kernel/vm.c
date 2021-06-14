#include "syscall.h"


int main(void) {
  	int cnt = 0;
  	if(fork() == 0) {
    	fork();
    	fork();
		while(cnt < 10) {
			uart_printf("pid: %d, sp: 0x%x cnt: %d\n", getpid(), &cnt, cnt); // address should be the same, but the cnt should be increased indepndently
			cnt++;
			int time_cnt = 100000;
			while(time_cnt--){}
		}
	} else {
		int* a = 0x0; // a non-mapped address.
		uart_printf("%d\n", *a); // trigger simple page fault.
		uart_printf("Should not be printed\n");
	}
	exit();
}