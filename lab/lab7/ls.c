#include "user.h"

int main(int argc, char** argv) {
  int fd = open(argv[1], 0);
  char name[100], buf[100];
  int size;
  // Modify the for loop to iterate the directory entries of the opened
  // directory.
  for (int i = 0; i < DIR_MAX; ++i) {
    size = readdir(argv[1], name, i);
    if (size > 0) {
      _uart_puts("Name: ");
      _uart_puts(name);
      _uart_puts(" Size: ");
      _uart_puts(itoa(size, buf));
      _uart_puts("\n");
    }
  }
}