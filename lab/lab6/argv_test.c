#include "user.h"
#define BUFF_SIZE 64
void test();
void main(int argc, char **argv) {
  _uart_puts("Argv Test, pid ");
  getpid();
  _uart_puts("\n");

  for (int i = 0; i < argc; ++i) {
    _uart_puts(argv[i]);
    _uart_puts("\n");
  }

  char *fork_argv[] = {"fork_test.img", "TEST_ARG_1", "TEST_ARG_2", 0};
  exec("fork_test.img", fork_argv);
}