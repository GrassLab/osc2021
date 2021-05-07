# include "user_demo.h"
# include "user_lib.h"

# include "uart.h"

const char *argv_test_argv[] = {"argv_test", "-o", "argv2", "argv3", "Hello~~~", 0};
const char *fork_test_argv[] = {"fork_test", 0};

void fork_test(int argc, char **argv){
  int cnt = 1;
  int ret = 0;
  char ct[20];
  uart_write((char *) "pid: ", 5);
  int_to_str(getpid(), ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) ", cnt: ", 7);
  int_to_str(cnt, ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) ", &cnt: ", 8);
  ptr_to_hex((void *)(&cnt), ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) "\n", 1);
  if ((ret = fork()) == 0){
    uart_write((char *) "pid: ", 5);
    int_to_str(getpid(), ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) ", cnt: ", 7);
    int_to_str(cnt, ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) ", &cnt: ", 8);
    ptr_to_hex((void*)(&cnt), ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) "\n", 1);
    cnt++;
    fork();
    while(cnt < 5){
      uart_write((char *) "pid: ", 5);
      int_to_str(getpid(), ct);
      uart_write(ct, str_len(ct));
      uart_write((char *) ", cnt: ", 7);
      int_to_str(cnt, ct);
      uart_write(ct, str_len(ct));
      uart_write((char *) ", &cnt: ", 8);
      ptr_to_hex((void *)(&cnt), ct);
      uart_write(ct, str_len(ct));
      uart_write((char *) "\n", 1);
      delay(1000000);
      cnt++;
    }
  }
  else{
    uart_write((char *) "parent here, pid ", 17);
    int_to_str(getpid(), ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) ", child ", 8);
    int_to_str(ret, ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) "\n", 1);
    delay(10000000);
  }
  exit();
}


void argv_test(int argc, char **argv){
  char ct[20];
  uart_write((char *) "Argv Test, pid: ", 16);
  int_to_str(getpid(), ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) "\n", 1);
  uart_write((char *) "argc: ", 5);
  int_to_str(argc, ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) "\n", 1);
  for (int i=0; i<argc; i++){
    uart_write(argv[i], str_len(argv[i]));
    uart_write((char *)"\n", 1);
  }
  exec(fork_test, (char **)fork_test_argv);
  exit();
}


void user_demo_test(){
  exec(argv_test, (char **)argv_test_argv);
  exit();
}
