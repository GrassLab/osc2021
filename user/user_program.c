#include "user_lib.h"
#include "printf.h"

void test_command1() { // test fork functionality
  int cnt = 0;
  if(fork() == 0) {
    fork();
    fork();
    while(cnt < 10) {
      printf("task id: %d, sp: 0x%x cnt: %d\r\n", get_taskid(), &cnt, cnt++); // address should be the same across tasks, but the cnt should be increased indepndently
      delay(1000000);
    }
    exit(); // all childs exit
  }
}

void test_command2() { // test page fault
  if(fork() == 0) {
    int* a = 0x0; // a non-mapped address.
    printf("%d\r\n", *a); // trigger simple page fault, child will die here.
  }
}

void test_command3() { // test page reclaim.
	printf("Remaining page frames : %d\r\n", remain_page_num()); // get number of remaining page frames from kernel by system call.
}




void user_main() {
	
	char cmd_buffer[100];
	char tmp[2];
	char *cmd_ptr = cmd_buffer;
	int parse_flag = 0;
	write("\r\n-------------------------\r\n");
    write("=      User  Space      =\r\n");
    write("-------------------------\r\n");
	write("# ");
	while(1) {
		read((unsigned long)cmd_ptr, 1);
		if (*cmd_ptr == '\r') {
			write("\r\n# ");
			parse_flag = 1;
		}
		else {
			tmp[0] = *cmd_ptr;
			tmp[1] = '\0'; 
			write((unsigned long)tmp);
		}
		if (parse_flag == 1) {
			*cmd_ptr = '\0';
			if (strcmp(cmd_buffer, "test1") == 0) {
				test_command1();
			}
			else if (strcmp(cmd_buffer, "test2") == 0) {
				test_command2();
			}
			else if (strcmp(cmd_buffer, "test3") == 0) {
				test_command3();
			}
			cmd_ptr = cmd_buffer;
			parse_flag = 0;
			
		}
		else {
			cmd_ptr++;
		}
	}
}
