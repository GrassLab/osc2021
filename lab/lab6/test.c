#include "user.h"
void main(int argc, char **argv) {
  long long int test;
  test = _uart_getc();
  test++;
  test = _uart_getc();
  test++;
  print_hex(test);
  print_hex((unsigned long)&test);
}