# include "user_lib.h"

const char *fork_test_argv[] = {"fork_test", 0};

void main(int argc, char **argv){
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
  exec("fork_test.img", (char **)fork_test_argv);
  exit();
}
