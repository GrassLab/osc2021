# include "user_demo.h"
# include "user_lib.h"

char *argv_demo[] = {"aaa", "aaaaa", "qqq", "\0", 0};

void exec_test(int argc, char **argv){
  char ct[20];
  uart_write((char *) "argc: ", 5);
  int_to_str(argc, ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) "\n", 1);
  exit();
}

void user_demo_test(){
  char ct[20];
  uart_write((char *) "pid: ", 5);
  int_to_str(getpid(), ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) "\n", 1);
  int cnt = 1;
  int ret = 0;
  //
  //char *argv[] = {"aaa", "aaaaa", "qqq", 0};
  exec(exec_test, argv_demo);
  exit();
  //
  if ((ret = fork()) == 0){
    uart_write((char *) "pid: ", 5);
    int_to_str(getpid(), ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) ", cnt: ", 7);
    int_to_str(cnt, ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) ", &cnt: ", 8);
    int_to_hex((unsigned long long)(&cnt), ct);
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
      int_to_hex((unsigned long long)(&cnt), ct);
      uart_write(ct, str_len(ct));
      uart_write((char *) "\n", 1);
      delay(100000000);
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
    char *argv[4] = {"aaa", "aaaaa", "qqq", "\0"};
    exec(exec_test, argv);
  }
  exit();
}

