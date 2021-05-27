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

void test_command4() {
	sys_print_debug();
}

void test_command5() {
	int cnt = 0;
	if(fork() == 0) {
		while(cnt < 10) {
        	printf("task id: %d, sp: 0x%x cnt: %d\r\n", get_taskid(), &cnt, cnt++); // address should be the same across tasks, but the cnt should be increased indepndently
      		delay(1000000);
    	}
    	exit(); // all childs exit
	}
}

void test_command6() {
	char buf[100];
	int a = fopen("hello", REG_FILE | O_CREAT);
	int b = fopen("world", REG_FILE | O_CREAT);
	fwrite(a, "Hello ", 6);
	fwrite(b, "World!", 6);
	fclose(a);
	fclose(b);
	b = fopen("hello", REG_FILE);
	a = fopen("world", REG_FILE);
	int sz;
	sz =  fread(b, buf, 100);
	sz += fread(a, buf + sz, 100);
	fclose(a);
	fclose(b);
	buf[sz] = '\0';
	printf("%s\r\n", buf); // should be Hello World!
}

void user_main() {
	char cmd_buffer[100];
	char tmp[2];
	char *cmd_ptr = cmd_buffer;
	int parse_flag = 0;
	while(1) {
		read((unsigned long)cmd_ptr, 1);
		if (*cmd_ptr == '\r') {
			write("\r\n");
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
      		else if (strcmp(cmd_buffer, "test4") == 0) {
        		test_command4();
      		}
			else if (strcmp(cmd_buffer, "test5") == 0) {
				test_command5();
			}
			else if (strcmp(cmd_buffer, "test6") == 0) {
				test_command6();
			}
			cmd_ptr = cmd_buffer;
			parse_flag = 0;
		}
		else {
			cmd_ptr++;
		}
	}
}